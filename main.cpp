

chip8 myChip;


int main() {
  /*
    do graphics
    setup render system and register input callbacks
  */
  //initialize 
  myChip.init();
  //load game
  //myChip.loadGame("pong");
  //emu loop
  while(true) {
    myChip.emulateCycle();
    if (myChip.df) {
      //draw
    }
    // store key press state
    myChip.setKeys();
}
return 0;
}
