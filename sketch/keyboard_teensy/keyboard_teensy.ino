const int number_of_pins = 24;
//                                  B7 D0 D1 D2 D3 D4 D5 D6 D7 E0 E1 C0 C1 C2 C3 C4 C5 C6 C7 E6 E7 B0 B1 B2  B3 B4 B5 B6
//first version
//int pins_teensy[number_of_pins] =  {27, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22}; // ,23,24,25,26};
int pins_arduino[number_of_pins] = { 2, 3, 4, 5, 6, 7, 8, 9,10,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50};
int pins_teensy[number_of_pins] =  {20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,25,24,23,22,21}; 
//^ as connected to teensy - v2
int pins_arduino2teensy[51]; // populated dynamically
//Pins B7/27, B6/26 do not work

//ipin - initial pin (used on ATMega2560, but not on teensy)
//int key_matrix_ipins[number_of_pins] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50};

const int key_matrix_x_ipins_count = 8;
int key_matrix_x_ipins[key_matrix_x_ipins_count] = {26, 28, 30, 32,38, 40, 42, 44 };

const int key_matrix_y_ipins_count = 14;
int key_matrix_y_ipins[key_matrix_y_ipins_count] = { 2, 3, 4, 5, 6, 7, 9, 10, 22, 34, 36, 46, 48, 50};

unsigned int key_matrix_by_ipins[key_matrix_y_ipins_count][key_matrix_x_ipins_count] = {
// tpins         9,                10,                11,        12,             13,        14,     15,              16,            17,        18,         19,     20,     21,            22
// ipin:        24,                26,                28,        30,             32,        34,     36,              38,            40,        42,         44,     46,     48,            50, 

// tpin-ipin
// tpins                   10,                11,        12,             13,              16,            17,        18,         19,
// ipin:                   26,                28,        30,             32,              38,            40,        42,         44, 
/* 27  2 */   KEY_PRINTSCREEN,     KEY_BACKSLASH,  KEY_DOWN,         KEY_UP,               0, KEY_BACKSPACE, KEY_EQUAL,  KEY_PAUSE, 
/*  0  3 */           KEY_F11,       KEY_PAGE_UP,     KEY_U,      KEY_ENTER, KEY_RIGHT_BRACE,      KEY_HOME, KEY_MINUS,    KEY_F12, 
/*  1  4 */           KEY_F10,     KEY_PAGE_DOWN, KEY_QUOTE, KEY_LEFT_BRACE,         KEY_END,         KEY_0,     KEY_9,     KEY_F9, 
/*  2  5 */        KEY_INSERT,          KEY_LEFT, KEY_SLASH,  KEY_SEMICOLON,       KEY_RIGHT,         KEY_P,     KEY_8, KEY_DELETE, 
/*  3  6 */ MODIFIERKEY_SHIFT, MODIFIERKEY_SHIFT,         0,              0,               0,             0,         0,          0, 
/*  4  7 */  MODIFIERKEY_CTRL,  MODIFIERKEY_CTRL,         0,              0,               0,             0,         0,          0, 
/*  6  9 */         KEY_COMMA,        KEY_PERIOD,     KEY_J,          KEY_L,           KEY_K,         KEY_O,     KEY_I,     KEY_F8, 
/*  7 10 */   MODIFIERKEY_ALT,   MODIFIERKEY_ALT,         0,              0,               0,             0,         0,          0, 
/*  8 22 */                 0,   MODIFIERKEY_GUI,         0,              0,               0,             0,         0,          0, 
/* 14 34 */             KEY_Y,             KEY_G, KEY_SPACE,          KEY_N,           KEY_V,         KEY_B,     KEY_H,      KEY_7, 
/* 15 36 */             KEY_6,             KEY_T,     KEY_Z,          KEY_X,           KEY_C,             0,     KEY_F,     KEY_F7, 
/* 20 46 */            KEY_F5,             KEY_5,     KEY_S,          KEY_D,           KEY_R,         KEY_E,     KEY_4,     KEY_F6, 
/* 21 48 */            KEY_F4,            KEY_F3,     KEY_A,          KEY_Q,           KEY_2,         KEY_W,     KEY_3,     KEY_F2, 
/* 22 50 */           KEY_ESC,         KEY_TILDE,         0,          KEY_M,           KEY_1, KEY_CAPS_LOCK,   KEY_TAB,     KEY_F1, 
};
//MODIFIERKEY_CTRL:32769

const int g_teensy_modifiers_cnt = 4;
int g_teensy_modifiers[g_teensy_modifiers_cnt] = {MODIFIERKEY_CTRL, MODIFIERKEY_SHIFT, MODIFIERKEY_ALT, MODIFIERKEY_GUI};

int g_pin_OUTPUT_current = -1;

long g_cnt = 0;
int g_printed = 0;

const int g_pressed_keys_cnt_max = 6;
unsigned int g_pressed_keys[g_pressed_keys_cnt_max]; // buffer of pressed key - during one scan
int g_pressed_keys_pointer;

unsigned int g_pressed_keys_prev[g_pressed_keys_cnt_max]; // buffer of pressed key - during one scan
int g_pressed_keys_pointer_prev;
int g_pressed_keys_prev_initialized = 0;

unsigned int g_keys_diff_key[g_pressed_keys_cnt_max*2];
int g_keys_diff_type[g_pressed_keys_cnt_max*2]; // 1 - added(pressed), 0 - removed(released)
int g_keys_diff_pointer;

//usb buf
const int g_usb_buf_keys_cnt_max = 6;
unsigned int g_usb_buf_keys[g_usb_buf_keys_cnt_max];
int g_usb_buf_keys_pointer = 0;
const int g_usb_buf_modifiers_cnt_max = 4;
unsigned int g_usb_buf_modifiers[g_usb_buf_modifiers_cnt_max];
int g_usb_buf_modifiers_pointer = 0;
//unsigned int g_usb_buf_modifiers_field = 0;

int g_debug = 1;

long g_debug_pereiodic_dump_last_millis = 0;

int g_stop_signal = 0;
int g_pin_stop_signal = 18; // 18/E6

void setup() { 
  //Serial.begin(9600); // USB is always 12 Mbit/sec
  Serial.begin(57600); // USB is always 12 Mbit/sec
  Serial.println("Hello World.");
  //delay(6000); //put when send keys to host - to be protected from sending wrong keys to host!
  Serial.println("Protection delay passed.");

  for(int ipin_index = 0; ipin_index < number_of_pins; ipin_index++){
    int ipin = pins_arduino[ipin_index];
    int tpin = pins_teensy[ipin_index];
    pins_arduino2teensy[ipin] = tpin;
  }
  g_pressed_keys_pointer_prev = 0;
  for(int usb_i = 0; usb_i < g_usb_buf_keys_cnt_max; usb_i++){  
    g_usb_buf_keys[usb_i] = 0;
  }

  setup_pins();
  
  //check stop signal:
  pinMode(g_pin_stop_signal, INPUT_PULLUP);
  delay(20);
  int v = digitalRead(g_pin_stop_signal);
  if(v == LOW){
    Serial.println("Got stop signal (pin 27/B7 is LOW). Stopping.");
    g_stop_signal = 1;
  }
  //check stop signal.
/*
  Keyboard.set_key1(KEY_X);
  Keyboard.send_now();
  delay(100);
  Keyboard.set_key1(KEY_Y);
  Keyboard.set_key2(KEY_X);
  Keyboard.send_now();
  delay(100);
  Keyboard.set_key1(0);
  Keyboard.set_key2(0);
  Keyboard.send_now();
*/

} 

void loop() {
//  delay(611000);
  //delay(3000);
  g_cnt++;

  if(!g_stop_signal){
    loop_doit();
  }else{
     delay(50000);
  }
}
void loop_doit(){
  scan_all_pairs(); //pressed keys are in g_pressed_keys[] array. Count = g_pressed_keys_pointer
  analyse_buffer_and_act(); // fills g_keys_diff_key[], g_keys_diff_type[] arrays

  /*
  if (!g_printed && millis() > 1000){
    Serial.println( "cnt: " + String(g_cnt) );
    g_printed = 1;
  }*/
  if( millis() - g_debug_pereiodic_dump_last_millis > 10000){ //dump each 10 secs
    if(g_debug){ 
      debug_dump();
      Serial.println("millis: " + String (millis()) + " g_..dump_last_millis: " + String(g_debug_pereiodic_dump_last_millis) );
    }
    g_debug_pereiodic_dump_last_millis = millis();
  }
}

void setup_pins(){
  for(int pin_x_index = 0; pin_x_index < key_matrix_x_ipins_count; pin_x_index++){
    int ipin_x = key_matrix_x_ipins[pin_x_index];
    int tpin_x = pins_arduino2teensy[ipin_x];
    set_pin_INPUT(tpin_x);
  }
  for(int pin_y_index = 0; pin_y_index < key_matrix_y_ipins_count; pin_y_index++){
    int ipin_y = key_matrix_y_ipins[pin_y_index];
    int tpin_y = pins_arduino2teensy[ipin_y];
    set_pin_INPUT(tpin_y);
  }
}

void set_pin_INPUT(int tpin){
  pinMode(tpin, INPUT_PULLUP);  
}

void analyse_buffer_and_act(){
  g_keys_diff_pointer = 0;
  int found;
  unsigned int key;
  unsigned int key_prev;
  int key_prev_i;
  int key_i;
  
  //work
  if(g_pressed_keys_prev_initialized){
    //find diff in buffers

    //presented in prev, but absent in cur -> released keys
    for(key_prev_i = 0; key_prev_i < g_pressed_keys_pointer_prev; key_prev_i++){
      key_prev = g_pressed_keys_prev[key_prev_i];
      //find key in cur
      found = 0;
      for(key_i = 0; key_i < g_pressed_keys_pointer; key_i++){
        key = g_pressed_keys[key_i];
        if(key == key_prev){
          found=1; break;
        }
      }
      if(!found){
        g_keys_diff_key[g_keys_diff_pointer] = key_prev;
        g_keys_diff_type[g_keys_diff_pointer] = 0;
        g_keys_diff_pointer++;
      }
    }

    //presented in cur, but absent in prev -> pressed keys
    for(key_i = 0; key_i < g_pressed_keys_pointer; key_i++){
      key = g_pressed_keys[key_i];
      //find key in prev
      found = 0;
      for(key_prev_i = 0; key_prev_i < g_pressed_keys_pointer_prev; key_prev_i++){
        key_prev = g_pressed_keys_prev[key_prev_i];
        if(key == key_prev){
          found=1; break;
        }
      }
      
      if(!found){
        g_keys_diff_key[g_keys_diff_pointer] = key;
        g_keys_diff_type[g_keys_diff_pointer] = 1;
        g_keys_diff_pointer++;
      }
      
    }
    
    act_on_diff(); //inside if(g_pressed_keys_prev_initialized)
    
  }//work.
  
  //after work - copy buffer to prev
  for(key_i = 0; key_i < g_pressed_keys_pointer; key_i++){
      g_pressed_keys_prev[key_i] = g_pressed_keys[key_i];
  }
  g_pressed_keys_pointer_prev = g_pressed_keys_pointer;
  g_pressed_keys_prev_initialized = 1;

}


boolean key_is_modifier(unsigned int key){
  boolean ret = false;
//  const int g_teensy_modifiers_cnt = 4;
//  int g_teensy_modifiers[teensy_modifiers_cnt] = {MODIFIERKEY_CTRL, MODIFIERKEY_SHIFT, MODIFIERKEY_ALT, MODIFIERKEY_GUI};
  for(int i = 0; i < g_teensy_modifiers_cnt; i++){
    if(key == g_teensy_modifiers[i]){
      ret = true; break;
    }
  }
  return ret;
}

void scan_all_pairs(){
  
  //clear pressed keys buffer
  g_pressed_keys_pointer = 0;

  for(int pin_x_index = 0; pin_x_index < key_matrix_x_ipins_count; pin_x_index++){
    int ipin_x = key_matrix_x_ipins[pin_x_index];
    int tpin_x = pins_arduino2teensy[ipin_x];
    set_only_one_pin_OUTPUT(tpin_x);
    for(int pin_y_index = 0; pin_y_index < key_matrix_y_ipins_count; pin_y_index++){
      int ipin_y = key_matrix_y_ipins[pin_y_index];
      int tpin_y = pins_arduino2teensy[ipin_y];
      check_one_pair(tpin_x,tpin_y,pin_x_index,pin_y_index);
    }
  }
  if(g_pressed_keys_pointer > 0){
    //String s1="Pressed:\r\n";
    for(int key_i = 0; key_i < g_pressed_keys_pointer; key_i++){
      int key = g_pressed_keys[key_i];
      //s1 += String(key) + "\r\n";
    }
    //Serial.print(s1);
  }
}

//tpin_x is already set with OUTPUT LOW
void check_one_pair(int tpin_x, int tpin_y, int pin_x_index, int pin_y_index){ 
  int v = digitalRead(tpin_y);
  if(v == LOW){
    //add  pressed key to array of pressed keys
    unsigned int key = key_matrix_by_ipins[pin_y_index][pin_x_index];
    //Serial.println("shorted tpins tpx:"+String(tpin_x)+" tpy:" + String(tpin_y) + " pxi:"+String(pin_x_index)+" pyi:" + String(pin_y_index) + " k:" + String(key) + 
    // " pointer:" + String(g_pressed_keys_pointer) );
    
    if(g_pressed_keys_pointer < g_pressed_keys_cnt_max){
      g_pressed_keys[g_pressed_keys_pointer] = key;
      g_pressed_keys_pointer++;
    }else{
      //skip
      //this can generate a lot of traffic, as we come here on each loop, when > 6 buttons are pressed.
      //if(g_debug) Serial.println("err.Pressed key " + String(key) + " is skipped, as pressed buf is full. pxi:" + String(pin_y_index) + " pyi:"+ String(pin_y_index) );
    }
  }
}

void set_all_pins_as_INPUT(){
  for(int pin_x_index = 0; pin_x_index < key_matrix_x_ipins_count; pin_x_index++){
    int ipin = key_matrix_x_ipins[pin_x_index];
    int tpin = pins_arduino2teensy[ipin];
    pinMode(tpin, INPUT);
  }
  for(int pin_y_index = 0; pin_y_index < key_matrix_y_ipins_count; pin_y_index++){
    int ipin = key_matrix_y_ipins[pin_y_index];
    int tpin = pins_arduino2teensy[ipin];
    pinMode(tpin, INPUT);
  }
}

void set_only_one_pin_OUTPUT(int tpin){
  if(g_pin_OUTPUT_current != -1){
    pinMode(g_pin_OUTPUT_current, INPUT_PULLUP);
  }
  pinMode(tpin, OUTPUT);
  digitalWrite(tpin, LOW);
  
  g_pin_OUTPUT_current = tpin;
}


void act_on_diff(){
  
  if(g_keys_diff_pointer > 0){ //something is pressed/released in this circle. Number of changes can be more than usb_buffer length as of release/press
    String s1="Diff:\r\n";
    for(int key_diff_i = 0; key_diff_i < g_keys_diff_pointer; key_diff_i++){
      unsigned int key = g_keys_diff_key[key_diff_i];
      int act = g_keys_diff_type[key_diff_i];
      boolean is_modifier = key_is_modifier(key);
//    unsigned int g_usb_buf_keys[g_usb_buf_keys_cnt_max];
//    unsigned int g_usb_buf_modifiers[g_usb_buf_modifiers_cnt_max];
      if(!is_modifier){
        //not modifier
        if(act==1){
          //add key
          if(g_usb_buf_keys_pointer < g_usb_buf_keys_cnt_max){
            g_usb_buf_keys[g_usb_buf_keys_pointer] = key;
            g_usb_buf_keys_pointer++;
          }else{
            //skipping pressed key
            if(g_debug) Serial.println("err.usb_buf_keys is full");
          }
        }else{ //act=0
          //remove key
          remove_key_from_usb_buf(key);
        }
      }else{//modifier
        if(act==1){
          //add modifier
          if(g_usb_buf_modifiers_pointer < g_usb_buf_modifiers_cnt_max){
            g_usb_buf_modifiers[g_usb_buf_modifiers_pointer] = key;
            g_usb_buf_modifiers_pointer++;
          }else{
            //skipping pressed modifier
            if(g_debug) Serial.println("err.usb_buf_modifiers is full");
          }
        }else{ //act=0
          //remove modifier
          remove_modifier_from_usb_buf_modifiers(key);
        }
      }
      //one key is updated in usb_buf
      usb_buf_send_to_host();
      s1 += "key:"+String(key) + " act:"+String(act) + "\r\n";
    }
    Serial.print(s1);
  }
}

void usb_buf_send_to_host(){
  unsigned int modifiers_field;
  modifiers_field = 0;
  for(int i=0; i < g_usb_buf_modifiers_pointer; i++){
      modifiers_field |= g_usb_buf_modifiers[i];
  }
  Keyboard.set_key1( g_usb_buf_keys[0] );
  Keyboard.set_key2( g_usb_buf_keys[1] );
  Keyboard.set_key3( g_usb_buf_keys[2] );
  Keyboard.set_key4( g_usb_buf_keys[3] );
  Keyboard.set_key5( g_usb_buf_keys[4] );
  Keyboard.set_key6( g_usb_buf_keys[5] );
  Keyboard.set_modifier(modifiers_field);
  Keyboard.send_now();
  //delay(1000);
}

void remove_key_from_usb_buf(unsigned int key){
  //find index in usb buf
  int released_key_index = -1;
  for(int usb_i = 0; usb_i < g_usb_buf_keys_pointer; usb_i++){
    if(g_usb_buf_keys[usb_i] == key){
      released_key_index = usb_i;
    }
  }
  if(released_key_index != -1){
    for(int ii=released_key_index; ii < g_usb_buf_keys_pointer-1; ii++){
        g_usb_buf_keys[ii] = g_usb_buf_keys[ii+1];
    }
    g_usb_buf_keys_pointer--;
    g_usb_buf_keys[g_usb_buf_keys_pointer] = 0;
  }else{
     if(g_debug) Serial.println("err.released key "+ String(key) + " is not found in usb_buf");
  }
}

void remove_modifier_from_usb_buf_modifiers(unsigned int key){
  //find index in usb buf
  int released_key_index = -1;
  for(int usb_i = 0; usb_i < g_usb_buf_modifiers_pointer; usb_i++){
    if(g_usb_buf_modifiers[usb_i] == key){
      released_key_index = usb_i;
    }
  }
  if(released_key_index != -1){
    for(int ii=released_key_index; ii < g_usb_buf_modifiers_pointer-1; ii++){
        g_usb_buf_modifiers[ii] = g_usb_buf_modifiers[ii+1];
    }
    g_usb_buf_modifiers_pointer--;
    g_usb_buf_modifiers[g_usb_buf_modifiers_pointer] = 0;
  }else{
     if(g_debug) Serial.println("err.released modifier "+ String(key) + " is not found in usb_buf_modifiers");
  }
}

void debug_dump(){
  String s1="Dump:\r\n";
  s1+="USB-Pressed keys: ";
  if(g_usb_buf_keys_pointer>0){
    for(int usb_i = 0; usb_i < g_usb_buf_keys_pointer; usb_i++){
      s1 += String(g_usb_buf_keys[usb_i]) + " ";
    }
  }else{
    s1+=" <none>";
  }
  s1+="\r\n";
  s1+="g_usb_buf_keys: ";
  for(int usb_i = 0; usb_i < g_usb_buf_keys_cnt_max; usb_i++){
    s1 += String(g_usb_buf_keys[usb_i]) + " ";
  }
  s1+= "\r\n";
  s1+= "USB-Pressed g_usb_buf_modifiers_field: ";
  for(int usb_i = 0; usb_i < g_usb_buf_modifiers_cnt_max; usb_i++){
    s1 += String(g_usb_buf_modifiers[usb_i]) + " ";
  }
  s1+= "\r\n";
  Serial.print(s1);
}
