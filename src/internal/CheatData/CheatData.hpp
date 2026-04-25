#include <windows.h>

namespace g_CheatData {
	namespace VTable {
		namespace D3D12 {
			int Present = 8;
			int ExecuteCommandLists = 10;
			int ResizeBuffers = 13;
		}
	}

	namespace Signature {
		namespace UWorld {
			// 7FF7A935DA20 World Tick?
			// 7FF7A935DC40 World Post Tick?
			// 2026/4/3 @zetsr
			// ? ? ? ? ? 57 48 83 EC ? 48 8B 42 ? 49 8B E8 48 89 5C 24 ? 4C 8B E9 48 89 74 24 ? 48 8B 2026/4/3 过期了

			// 2026/4/3 @zetsr
			// push rbp push r14 push r15 sub rsp, offset mov rax, [rdx + offset] mov eax, [rdx + offset] mov [rsp + offset], rbx mov ebp, ecx mov [rsp + offset], rsi mov rax, [rdx]

            // 2026/4/26 @zetsr & @Lynneren
            // push rbx; push rbp; push rsi; push rdi; push r12; push r14; push r15; sub rsp, sz; mov rax, [cookie]; xor rax, rsp; mov [rsp+sz], rax; mov r14, rcx; mov r15, r8; lea rcx, [rsp+30h]
            std::string Tick = "40 53 55 56 57 41 54 41 56 41 57 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 ?? ?? ?? ?? 4C 8B F1 4D 8B F8 48 8D 4C 24 30 48";

			namespace UNetDriver {
				namespace UNetConnection {
					// 7FF7A8D25520
					std::string HandleDisconnect = "? ? ? ? ? ? ? 55 57 48 8D 68 A1 48 81 EC D8 00 00 00 48 89 58 E8 48 8D 55 E7 48 89 70 E0";
				}
			}

            std::string PhysicsRotation = "48 89 5C 24 ? 48 89 7C 24 ? 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 44 0F 29 BC 24";
		}

		namespace UEngine {
			namespace UGameViewportClient {
				// https://github.com/vsylva/ark-asa-internal
				// VTable = 122，不知道这个虚表会不会过期，可能还是需要做特征码
				// mov rax, [rcx] jmp [rax + offset] ... mov [rsp + offset], rbp mov [rsp + offset], rdi push r12 push r14
				std::string PostRender = "48 8B 01 48 FF A0 ? ? ? ? ? ? ? ? ? ? 48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56";

				namespace UConsole {
					// 2026/4/3 @zetsr
					// 48 8B C4 ? ? 48 83 EC ? 48 89 58 ? 48 8B DA 2026/4/3 过期了
					
					// mov [rsp + offset], rbx push rbp push rsi push rdi push r12 push r13 push r14 push r15 mov rax, [rcx] sub rsp, offset movsxd rax, [rdx + offset] xor r14d, r14d mov rax, [rcx]
					std::string OutputTextLine = "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B ? 48 83 EC ? 48 63 ? ? 45 33 F6 48 8B";
				}
			}
		}
	}
}
