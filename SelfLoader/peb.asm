GLOBAL _slGetPEB;
GLOBAL _slGetWow32Reserved
SECTION .text;

_slGetPEB:
%ifdef BUILD_AMD64
	mov rax, [gs:0x60]
%else
	mov eax, [fs:0x30]
%endif
ret

_slGetWow32Reserved:
%ifdef BUILD_AMD64
	mov rax, [gs:0xF0]
%else
	mov eax, [fs:0xC0]
%endif
ret

