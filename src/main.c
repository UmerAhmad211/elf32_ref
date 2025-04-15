#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	Elf32_Ehdr header = { .e_ident = { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
					   ELFCLASS32, ELFDATA2LSB, EV_CURRENT,
					   ELFOSABI_SYSV, 0, 0, 0, 0, 0, 0, 0,
					   0 },
			      .e_type = ET_EXEC,
			      .e_machine = EM_386,
			      .e_version = EV_CURRENT,
			      .e_entry = 0x08048074,
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
				.p_offset = 0x74,
				.p_vaddr = 0x08048074,
				.p_paddr = 0,
				.p_filesz = 32,
				.p_memsz = 32,
				.p_flags = PF_X | PF_R,
				.p_align = 0x1000 };

	Elf32_Phdr phdr_data = { .p_type = PT_LOAD,
				 .p_offset = 0xa0,
				 .p_vaddr = 0x080480a0,
				 .p_paddr = 0,
				 .p_filesz = 18,
				 .p_memsz = 18,
				 .p_flags = PF_W | PF_R,
				 .p_align = 0x1000 };

	// clang-format off
	uint8_t objcode[] = {
		0xb8, 0x04, 0,	  0,	0, 		// mov eax, 4
		0xbb, 0x01, 0,	  0,	0, 		// mov ebx, 1
		0xb9, 0xa0, 0x80, 0x04, 0x08, 		// mov ecx, msg
		0x8b, 15,   0xae, 0x80, 0x04, 0x08, 	// mov edx, [len]
		0xcd, 0x80, 				// int 80h
		0xb8, 0x01, 0,	  0,	0, 		// mov eax, 1
		0x31, 0xdb, 				// xor ebx, ebx
		0xcd, 0x80 				// int 80h
	};
	// clang-format on

	// Hello, world!\n
	uint8_t data[] = {
		0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f,
		0x72, 0x6c, 0x64, 0x21, 0x0a, 0x0e, 0,	  0,	0,
	};

	FILE *file_w = fopen("a.out", "w");
	if (file_w == NULL) {
		perror("Error: Failed to open file.");
		exit(EXIT_FAILURE);
	}

	size_t sz = fwrite(&header, 1, sizeof(header), file_w);
	if (sz != sizeof(header)) {
		perror("Error: Failed to write ELF header.");
		exit(EXIT_FAILURE);
	}

	sz = fwrite(&phdr_txt, 1, sizeof(phdr_txt), file_w);
	if (sz != sizeof(phdr_txt)) {
		perror("Error: Failed to write ELF program header for text.");
		exit(EXIT_FAILURE);
	}

	sz = fwrite(&phdr_data, 1, sizeof(phdr_data), file_w);
	if (sz != sizeof(phdr_data)) {
		perror("Error: Failed to write ELF program header for data.");
		exit(EXIT_FAILURE);
	}

	sz = fwrite(objcode, 1, sizeof(objcode), file_w);
	if (sz != sizeof(objcode)) {
		perror("Error: Failed to write text.");
		exit(EXIT_FAILURE);
	}

	sz = fwrite(data, 1, sizeof(data), file_w);
	if (sz != sizeof(data)) {
		perror("Error: Failed to write data.");
		exit(EXIT_FAILURE);
	}

	fclose(file_w);

	return EXIT_SUCCESS;
}
