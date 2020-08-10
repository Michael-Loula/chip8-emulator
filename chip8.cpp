#include "chip8.h"
#include <iostream>
#include <random>

void chip8::init() {
  //Initialize regs and mem
  I = 0;
  pc = 0;
  sp = 0;
  opcode = 0;
  delay_timer = 0;
  sound_timer = 0;
  for (int i = 0; i < 4096; i++)
    memory[i] = 0;
  for (int i = 0; i < 16; i++) {
    V[i] = 0;
    stack[i] = 0;
    key[i] = 0;
  }
  for (int i = 0; i < 80; i++)
    memory[i + 0x050] = chip8_fontset[i];
  for (int i = 0; i < (64*32); i++)
    gfx[i] = 0;
}

void chip8::emulateCycle() {
  //Fetch opcode
  //this shifts first byte left 8 and ORs it w next byte
  opcode = memory[pc] << 8 | memory[pc+1];
  //Decode
  switch(opcode & 0xF000) {
  case 0xA000:
    I = opcode & 0x0FFF;
    pc += 2;
    break;
  case 0x0000:
    switch(opcode & 0x000F) {
    case 0x0000:
      for (int i = 0; i < 64*32; i++)
        gfx[i] = 0;
      break;
    case 0x000E:
      pc = stack[sp];
      sp -= 1;
      break;
    default:
      std::cout << std::hex << opcode;
    }
  case 0x1000:
    pc = 0x0FFF & opcode;
    break;
  case 0x2000:
    sp += 1;
    stack[sp] = pc;
    pc = 0x0FFF & opcode;
    break;
    case64;42;34M 0x3000:
    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x0FF)) pc += 2;
    pc += 2;
    break;
  case 0x4000:
    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x0FF)) pc += 2;
    pc += 2;
    break;
  case 0x5000:
    if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) pc += 2;
    pc += 2;
    break;
  case 0x6000:
    V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
    pc += 2;
    break;
  case 0x7000:
    V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
    pc += 2;
    break;
  case 0x8000:
    switch(opcode & 0x000F) {
    case 0x0000:
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0001:
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0002:
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0003:
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0004:
      V[16] = (V[(opcode & 0x0F00) >> 8] > (0x00FF - V[(opcode & 0x00F0) >> 4]));
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0005:
      V[16] = (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]);
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] - V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0006:
      V[16] = V[(opcode & 0x0F00) >> 8] & 0x000F;
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
      pc += 2;
      break;
    case 0x0007:
      V[16] = (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]);
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x000E:
      V[16] = V[(opcode & 0x0F00) >> 8] & 0xF000;
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
      pc += 2;
      break;
    }
    break;
  case 0xB000:
    pc = V[0] + (opcode & 0x0FFF);
    break;
  case 0xC000:
    V[(opcode & 0x0F00) >> 8] == (rand() % 256) & (opcode & 0x00FF);
    pc += 2;
    break;
  case 0xD000:
    for (int i = 0; i < (0x000F & opcode); i++) {
      for (int j = 0; j < 8; j++) {
        gfx[(V[(opcode & 0x0F00) >> 8] + j)*(V[(opcode & 0x00F0) >> 4]) + i] =
          memory[I+i+j];
        V[16] = memory[I+i+j] = 1 &&
          gfx[(V[(opcode & 0x0F00) >> 8] + j)*(V[(opcode & 0x00F0) >> 4]) + i] = 0;
        }
    }
    pc += 2;
    break;
  case 0xE000:
    switch(0x00FF) {
    case 0x009E:
      if (key[V[(opcode & 0x0F00) >> 8]] != 0) pc += 2;
      pc += 2;
      break;
    case 0x00A1:
      if (key[V[(opcode & 0x0F00) >> 8]] == 0) pc += 2;
      pc += 2;
      break;
    }
    break;
  case 0xF000:
    switch(0x00FF) {
    case 0x0007:
      V[(opcode & 0x0F00) >> 8] = delay_timer;
      pc += 2;
      break;
    case 0x000A:
      std::cin.get(V[(opcode & 0x0F00) >> 8]);
      pc + = 2;
      break;
    case 0x0015:
      delay_timer = V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x0018:
      sound_timer = V[(opcode & 0x0F00) >> 8];
      break;
    case 0x001E:
      I += V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x0029:
      I = 0x050 + 20*V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x0033:
      memory[I] = V[(opcode & 0x0F00) >> 8] / 100;;
      memory[I+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
      memory[I+2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
      pc +=2 ;
      break;
    case 0x0055:
      for (int i = 0; i < (opcode & 0x0F00); i++) {
        memory[I+i] = V[i];
      }
      pc += 2;
      break;
    case 0x0065:
     for (int i = 0; i < (opcode & 0x0F00); i++) {
        V[i] = memory[I+i];
      }
      pc += 2;
      break;
    }
    break;
  default:
     std::cout << std::hex << opcode;
  }
  //Update Timers
  if (delay_timer > 0) delay_timer--;
  if (sound_timer > 0) sound_timer--;
}


