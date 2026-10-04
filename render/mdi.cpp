#include "mdi.h"

void mdi::DrawIndirectBuffer(LocalObjectStorage::IndirectDrawBuffers* idb, uint32_t ov_prgm)
{
	idb->Bind();
	for (int i = 0; i < idb->command_count; i++)
	{
		if (ov_prgm == -1)
			glUseProgram(idb->commands[i].program);
		else
			glUseProgram(ov_prgm);
		idb->commands[i].mb->BindVa();

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)(idb->commands[i].start * sizeof(DrawElementsIndirectCommand)), idb->commands[i].count, 0);
	}
}
