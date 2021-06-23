.386p
		NAME    vesatst
		EXTRN   _MEM_SC :BYTE
		EXTRN   _width_size :BYTE
		EXTRN   _VESA_selector :BYTE
		EXTRN   _vid_ptr :BYTE
DGROUP          GROUP   _DATA
_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA           ENDS

_TEXT           SEGMENT PARA PUBLIC USE32 'CODE'
		ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
		PUBLIC  _psetPixel
		PUBLIC  _getPixel
		PUBLIC  _Virtual_Clear
_psetPixel:     push    ebp
		mov     ebp,esp
		mov     ax,word ptr _VESA_selector
		mov     fs,ax
		mov     ecx,dword ptr _vid_ptr
		mov     eax,dword ptr +0cH[ebp]
		mov     ebx,dword ptr +8H[ebp]
		mov     edx,dword ptr _width_size
		mul     edx
		mov     dl,byte ptr +10H[ebp]
		mov     byte ptr fs:[ecx+ebx+eax],dl
		pop     ebp
		ret
_getPixel:      push    ebp
		mov     ebp,esp
		mov     ax,word ptr _VESA_selector
		mov     fs,ax
		mov     ecx,dword ptr _vid_ptr
		mov     eax,dword ptr +0cH[ebp]
		mov     ebx,dword ptr +8H[ebp]
		push    edx
		mov     edx,dword ptr _width_size
		mul     edx
		add     ebx,eax
		pop     edx
		mov     eax,dword ptr fs:[ecx+ebx]
		and     eax,000000ffH
		pop     ebp
		ret
_Virtual_Clear: mov     ax,word ptr _VESA_selector
		mov     fs,ax
		mov     edx,edi
		cld
		mov     edi,dword ptr _vid_ptr
		mov     ecx,dword ptr _MEM_SC
		shr     ecx,02H
		push    es
		mov     ax,word ptr _VESA_selector
		mov     es,ax
		xor     eax,eax
		repe    stosd
		pop     es
		mov     edi,edx
		ret
_TEXT           ENDS

		END
