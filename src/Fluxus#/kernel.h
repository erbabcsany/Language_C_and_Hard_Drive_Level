//
// Created by ervin on [2026. 03. 04.].
//

#ifndef KERNEL_H
#define KERNEL_H

unsigned char inb(unsigned short port);
void draw_particle(int x, int y, char c, unsigned char color);
void print_string(int x, int y, const char* str, unsigned char color);
void clear_screen();
void kernel_main();

#endif //KERNEL_H