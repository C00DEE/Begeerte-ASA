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
		namespace AActor {
			std::string TakeDamage = "C2 00 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 48 8B 01 41 B0 01 33 D2 48 FF A0 ?? ?? ?? ??";
		}

		namespace UWorld {
			// 7FF7A935DA20 World Tick?
			// 7FF7A935DC40 World Post Tick?
			// 2026/4/3 @zetsr
			// ? ? ? ? ? 57 48 83 EC ? 48 8B 42 ? 49 8B E8 48 89 5C 24 ? 4C 8B E9 48 89 74 24 ? 48 8B 2026/4/3 过期了

			// 2026/4/3 @zetsr
			// push rbp push r14 push r15 sub rsp, offset mov rax, [rdx + offset] mov eax, [rdx + offset] mov [rsp + offset], rbx mov ebp, ecx mov [rsp + offset], rsi mov rax, [rdx]

            // 2026/4/26 @zetsr & @Lynneren
            // push rbx; push rbp; push rsi; push rdi; push r12; push r14; push r15; sub rsp, sz; mov rax, [cookie]; xor rax, rsp; mov [rsp+sz], rax; mov r14, rcx; mov r15, r8; lea rcx, [rsp+30h]
            // 40 53 55 56 57 41 54 41 56 41 57 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 ?? ?? ?? ?? 4C 8B F1 4D 8B F8 48 8D 4C 24 30 48
			
			// 2026/5/2 @zetsr
			// mov r1x, [mem] | push rbp | push r14 | push r15 | sub rsp, alloc | mov rax, [rip+offset]
			// 4C 8B ? 55 41 56 41 57 48 81 EC ? ? 00 00 48 8B 05

			// 2026/5/11 @zetsr
			// push rbp; push r13; push r15; sub rsp, 0x40; mov rax, [rdx+0x20]; mov r11, [r8]
			// 40 55 41 55 41 57 48 83 EC ? 48 8B ? ? 49 8B

			// 2026/5/20 @zetsr
			// mov r11, rsp | push rbp | push r14 | push r15 | sub rsp, alloc | mov rax, [rip+offset]
			// 4C 8B DC 55 41 56 41 57 48 81 EC 80 08 00 00 48 8B 05 CA 9B D4 08

			// 2026/5/23 @zetsr
            // push rbx | push rbp | push rsi | push rdi | push r12 | push r14 | push r15 | sub rsp, alloc | mov rax, [rip+offset] | xor rax, rsp | mov [rsp+offset], rax | mov r14, rcx | mov r15, r8 | lea rcx, [rsp+offset]
			// 40 53 55 56 57 41 54 41 56 41 57 48 81 EC 60 08 00 00 48 8B 05 27 B8 AC 08 48 33 C4 48 89 84 24 50 08 00 00 4C 8B F1 4D 8B F8 48 8D 4C 24 30
			std::string Tick = "40 53 55 56 57 41 54 41 56 41 57 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 ?? ?? ?? ?? 4C 8B F1 4D 8B F8 48 8D 4C 24 ??";

			namespace UNetDriver {
				namespace UNetConnection {
					// 7FF7A8D25520
					std::string HandleDisconnect = "? ? ? ? ? ? ? 55 57 48 8D 68 A1 48 81 EC D8 00 00 00 48 89 58 E8 48 8D 55 E7 48 89 70 E0";
				}
			}

			// 48 89 5C 24 ? 48 89 7C 24 ? 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 44 0F 29 BC 24 过期了
			// 2026/5/20 @zetsr
            // mov rax, rsp | push rbx | sub rsp, alloc | movaps [rax+offset], xmm6 | mov rbx, rcx | movaps [rax+offset], xmm7 | movaps
			// 48 8B C4 53 48 81 EC 00 01 00 00 0F 29 70 E8 48 8B D9 0F 29 78 D8 0F 28
            std::string PhysicsRotation = "48 8B C4 53 48 81 EC ?? ?? ?? ?? 0F 29 70 ?? 48 8B D9 0F 29 78 ?? 0F 28";
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
					// 48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B ? 48 83 EC ? 48 63 ? ? 45 33 F6 48 8B

					// 2026/5/11 @zetsr
					// mov rax, rsp; push r12; sub rsp, 0x60; mov [rax+0x08], rbx; mov rbx, rdx
					// 48 8B C4 41 54 48 83 EC ? 48 89 58 ? 48 8B ?

					// 2026/5/20 @zetsr
					// mov [rsp+8], rbx | push rbp | push rsi | push rdi | push r12 | push r13 | push r14 | push r15 | mov rbp, rsp | sub rsp, alloc | movsxd rsi, [rdx+offset] | xor r14d, r14d
					// 48 89 5C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 40 48 63 72 08 45 33 F6

					// 2026/5/23 @zetsr
					// mov rax, rsp | push r12 | sub rsp, alloc | mov [rax+8], rbx | mov rbx, rdx
					// 48 8B C4 41 54 48 83 EC 60 48 89 58 08 48 8B DA
					std::string OutputTextLine = "48 8B C4 41 54 48 83 EC ?? 48 89 58 08 48 8B DA";
				}
			}
		}
	}
}
