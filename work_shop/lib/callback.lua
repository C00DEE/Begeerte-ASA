local callback = {}

-- 全局持久化注册表
if not _G.__CALLBACK_REGISTRY then
    _G.__CALLBACK_REGISTRY = {
        events = {},
        source_map = {},
        pending_sources = {},
        active_dispatches = 0,
        shutting_down = false,
    }
end

local registry = _G.__CALLBACK_REGISTRY

local EVENT_NAMES = {
    "OnPaint",
    "OnPaintMenu",
    "OnWorldTick",
    "OnConsoleMessage",
    "OnDisconnect",
    "OnMenuOpen",
    "OnShutDown",
    "OnPostRender"
}

local function ensure_event(name)
    local e = registry.events[name]
    if not e then
        e = {
            list = {},   -- 有序回调节点数组
            index = {},  -- func -> entry
            dirty = false,
        }
        registry.events[name] = e
    end
    return e
end

for i = 1, #EVENT_NAMES do
    ensure_event(EVENT_NAMES[i])
end

local function safe_print_error(prefix, err)
    local msg = tostring(err)
    if type(print) == "function" then
        print(string.format("[%s] %s", prefix, msg))
    end
end

local function get_caller_source(level)
    local info = debug and debug.getinfo and debug.getinfo(level or 2, "S")
    if info and info.source then
        return info.source
    end
    return "unknown"
end

local function compact_event(e)
    if not e or not e.dirty then
        return
    end

    local new_list = {}
    local n = #e.list
    for i = 1, n do
        local entry = e.list[i]
        if entry and entry.active then
            new_list[#new_list + 1] = entry
        end
    end

    e.list = new_list
    e.dirty = false
end

local function remove_entry_from_source(source, entry)
    local bucket = registry.source_map[source]
    if not bucket then
        return
    end

    if bucket.entries[entry] then
        bucket.entries[entry] = nil
        bucket.count = bucket.count - 1
        if bucket.count <= 0 then
            registry.source_map[source] = nil
        end
    end
end

local function detach_entry(entry)
    if not entry or not entry.active then
        return
    end

    entry.active = false

    local e = registry.events[entry.event_name]
    if e and e.index[entry.func] == entry then
        e.index[entry.func] = nil
        e.dirty = true
    end

    remove_entry_from_source(entry.source, entry)

    if registry.active_dispatches == 0 and e then
        compact_event(e)
    end
end

local function purge_source_now(source)
    local bucket = registry.source_map[source]
    if not bucket then
        return
    end

    local entries = {}
    for entry in pairs(bucket.entries) do
        entries[#entries + 1] = entry
    end

    for i = 1, #entries do
        detach_entry(entries[i])
    end

    registry.source_map[source] = nil

    if registry.active_dispatches == 0 then
        for i = 1, #EVENT_NAMES do
            compact_event(registry.events[EVENT_NAMES[i]])
        end
    end
end

local function flush_pending_purges()
    if registry.active_dispatches ~= 0 then
        return
    end

    local pending = registry.pending_sources
    if not pending then
        return
    end

    local sources = {}
    for source in pairs(pending) do
        sources[#sources + 1] = source
    end

    for i = 1, #sources do
        local source = sources[i]
        pending[source] = nil
        purge_source_now(source)
    end

    for i = 1, #EVENT_NAMES do
        compact_event(registry.events[EVENT_NAMES[i]])
    end
end

local function unregister_entry(event_name, func)
    local e = registry.events[event_name]
    if not e then
        return false
    end

    local entry = e.index[func]
    if not entry then
        return false
    end

    detach_entry(entry)
    return true
end

local function init_hooks()
    for i = 1, #EVENT_NAMES do
        local name = EVENT_NAMES[i]
        local hook_flag = "__HOOKED_" .. name

        if not _G[hook_flag] then
            local original_event = _G[name]

            _G[name] = function(...)
                local is_shutdown_event = (name == "OnShutDown")

                registry.active_dispatches = registry.active_dispatches + 1

                if type(original_event) == "function" then
                    local ok, err = pcall(original_event, ...)
                    if not ok then
                        safe_print_error(name .. " original", err)
                    end
                end

                local e = registry.events[name]
                local snapshot_len = #e.list

                if is_shutdown_event then
                    registry.shutting_down = true
                end

                for idx = 1, snapshot_len do
                    local entry = e.list[idx]
                    if entry and entry.active then
                        local ok, err = pcall(entry.func, ...)
                        if not ok then
                            safe_print_error("Callback Error " .. name, err)

                            -- 单个回调出错，只移除自己，不连坐同源其它回调
                            detach_entry(entry)
                        end
                    end
                end

                registry.active_dispatches = registry.active_dispatches - 1

                if is_shutdown_event then
                    -- 关服时彻底清场
                    local sources = {}
                    for source in pairs(registry.source_map) do
                        sources[#sources + 1] = source
                    end

                    for i = 1, #sources do
                        purge_source_now(sources[i])
                    end

                    registry.pending_sources = {}
                    registry.shutting_down = false
                    return
                end

                if registry.active_dispatches == 0 then
                    flush_pending_purges()
                end
            end

            _G[hook_flag] = true
        end
    end
end

function callback.set(event_name, func)
    if registry.shutting_down then
        return false
    end

    if type(event_name) ~= "string" then
        return false
    end

    if type(func) ~= "function" then
        return false
    end

    local e = registry.events[event_name]
    if not e then
        return false
    end

    if e.index[func] then
        return false
    end

    local source = get_caller_source(2)

    local entry = {
        event_name = event_name,
        func = func,
        source = source,
        active = true,
    }

    e.list[#e.list + 1] = entry
    e.index[func] = entry

    local bucket = registry.source_map[source]
    if not bucket then
        bucket = {
            entries = {},
            count = 0,
        }
        registry.source_map[source] = bucket
    end

    if not bucket.entries[entry] then
        bucket.entries[entry] = true
        bucket.count = bucket.count + 1
    end

    return true
end

function callback.unset(event_name, func)
    if type(event_name) ~= "string" then
        return false
    end

    if type(func) ~= "function" then
        return false
    end

    return unregister_entry(event_name, func)
end

function callback.purge_source(source)
    if type(source) ~= "string" then
        return false
    end

    purge_source_now(source)
    return true
end

function callback.reset()
    for i = 1, #EVENT_NAMES do
        local e = registry.events[EVENT_NAMES[i]]
        e.list = {}
        e.index = {}
        e.dirty = false
    end

    registry.source_map = {}
    registry.pending_sources = {}
    registry.active_dispatches = 0
    registry.shutting_down = false

    return true
end

init_hooks()

return callback