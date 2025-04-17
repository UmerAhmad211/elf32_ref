#include <elf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_to_file(const void *data, FILE *file_w, const size_t sz,
		   const char *if_err_msg)
{
	if (fwrite(data, 1, sz, file_w) != sz) {
		perror(if_err_msg);
		fclose(file_w);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	Elf32_Ehdr header = { .e_ident = { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
					   ELFCLASS32, ELFDATA2LSB, EV_CURRENT,
					   ELFOSABI_SYSV, 0, 0, 0, 0, 0, 0, 0,
					   0 },
			      .e_type = ET_EXEC,
			      .e_machine = EM_386,
			      .e_version = EV_CURRENT,
			      .e_entry = 0x08049000,
			      .e_phoff = 0x34,
			      .e_shoff = 0,
			      .e_flags = 0,
			      .e_ehsize = 0x34,
			      .e_phentsize = 0x20,
			      .e_phnum = 2,
			      .e_shentsize = 0,
			      .e_shnum = 0,
			      .e_shstrndx = SHN_UNDEF };

	Elf32_Phdr phdr_txt = { .p_type = PT_LOAD,
				.p_offset = 0x0,
				.p_vaddr = 0x08048000,
				.p_paddr = 0,
				.p_filesz = 0x01020,
				.p_memsz = 0x01020,
				.p_flags = PF_X | PF_R,
				.p_align = 0x1000 };

	Elf32_Phdr phdr_data = { .p_type = PT_LOAD,
				 .p_offset = 0x2000,
				 .p_vaddr = 0x0804a000,
				 .p_paddr = 0,
				 .p_filesz = 18,
				 .p_memsz = 18,
				 .p_flags = PF_W | PF_R,
				 .p_align = 0x1000 };

	// clang-format off
	uint8_t objcode[] = {
		0xb8, 0x04, 0,	  0,	0, 		// mov eax, 4
		0xbb, 0x01, 0,	  0,	0, 		// mov ebx, 1
		0xb9, 0x00, 0xa0, 0x04, 0x08, 		// mov ecx, msg
		0x8b, 0x15,   0x0e, 0xa0, 0x04, 0x08, 	// mov edx, [len]
		0xcd, 0x80, 				// int 80h
		0xb8, 0x01, 0,	  0,	0, 		// mov eax, 1
		0x31, 0xdb, 				// xor ebx, ebx
		0xcd, 0x80 				// int 80h
	};
	// clang-format on

	// Hello, world!\n
	// len dd 14
	uint8_t data[] = {
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f,
		0x72, 0x6c, 0x64, 0x21, 0x0a, 0x0e, 0,	  0,	0,
	};

	FILE *file_w = fopen("a.out", "w");
	if (file_w == NULL) {
		perror("Error: Failed to open file.");
		exit(EXIT_FAILURE);
	}

	write_to_file(&header, file_w, sizeof(header),
		      "Error: Failed to write ELF header.");
	write_to_file(&phdr_txt, file_w, sizeof(phdr_txt),
		      "Error: Failed to write ELF program header for text.");
	write_to_file(&phdr_data, file_w, sizeof(phdr_data),
		      "Error: Failed to write ELF program header for data.");
	// 0x1000 - 0x32 - 0x20 - 0x20
	uint8_t pad[0x0f8c] = { 0 };
	write_to_file(pad, file_w, sizeof(pad), "Error: Failed to write pad.");
	write_to_file(objcode, file_w, sizeof(objcode),
		      "Error: Failed to write text.");
	// 0x2000 - 0x1020
	uint8_t pad_2[0xfe0] = { 0 };
	write_to_file(pad_2, file_w, sizeof(pad_2),
		      "Error: Failed to write pad.");
	write_to_file(data, file_w, sizeof(data),
		      "Error: Failed to write data.");
	fclose(file_w);

	return EXIT_SUCCESS;
}
