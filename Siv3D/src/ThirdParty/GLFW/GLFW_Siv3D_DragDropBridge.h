//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <stddef.h>

# ifdef __cplusplus
extern "C" {
# endif

enum GLFW_Siv3D_DragItemType
{
	GLFW_SIV3D_DRAG_ITEM_NONE = 0,
	GLFW_SIV3D_DRAG_ITEM_FILE_PATHS = 1,
	GLFW_SIV3D_DRAG_ITEM_TEXT = 2,
};

void GLFW_Siv3D_DragDropRegister(void* glfwWindow, void* dragDrop);
void GLFW_Siv3D_DragDropUnregister(void* glfwWindow);

int GLFW_Siv3D_DragDropAcceptsFilePaths(void* glfwWindow);
int GLFW_Siv3D_DragDropAcceptsText(void* glfwWindow);

void GLFW_Siv3D_DragDropSetDragOver(void* glfwWindow, int x, int y, int itemType);
void GLFW_Siv3D_DragDropClearDragOver(void* glfwWindow);

void GLFW_Siv3D_DragDropAppendFilePaths(void* glfwWindow, int x, int y, const char* const* paths, size_t count);
void GLFW_Siv3D_DragDropAppendText(void* glfwWindow, int x, int y, const char* text);

int GLFW_Siv3D_DragDropProcessMouseDragged(void* glfwWindow, void* nsView, void* nsEvent);
void GLFW_Siv3D_DragDropSourceEnded(void* glfwWindow);

# ifdef __cplusplus
}
# endif
