#include <LiquidCrystal.h>
#include <ShiftLCD.h>
#include <Keypad.h>

ShiftLCD lcd(2, 4, 3);

int hp = 13;
int ledY = 12;
int button = 15;
int reset = 14;

const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int   timer = 0;
char  code[5];
char  j_code[5];

void play_sound(int x, int y, int d1, int d2)
{
  for(int j=0;j<x;j++)
  {
    for(int i=0;i<=y;i++)
    {
      digitalWrite(hp,HIGH);
      delay(d1);
      digitalWrite(hp,LOW);
      delay(d1);
    }
    delay(d2);
  }
}

void aff_time(int t)
{
     lcd.print("   ");
     lcd.print(t / 600);
     lcd.print(t / 60 % 10);
     lcd.print(':');
     lcd.print(t % 60 / 10);
     lcd.print(t % 10);
}
 
void gen_code()
{
    int c = random(0, 10000);
    
    code[0] = (char)(c/1000+'0');
    code[1] = (char)(c/100%10+'0');
    code[2] = (char)(c/10%10+'0');
    code[3] = (char)(c%10+'0');
    code[4] = '\0';
}

void  reset_jcode(char c)
{
    j_code[0] = c;
    j_code[1] = c;
    j_code[2] = c;
    j_code[3] = c;
    j_code[4] = '\0';
}

int  verif_code(int tmps)
{
  if(code[0] == j_code[0]
    && code[1] == j_code[1]
    && code[2] == j_code[2]
    && code[3] == j_code[3])
    {
      digitalWrite(ledY, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bombe desamorcee");
      lcd.setCursor(0, 1);
      aff_time(tmps);
      delay(10000);
      lcd.clear();
      digitalWrite(ledY, LOW);
      return(1);
    }
  return(0);
}

int  sup_inf()
{
  int c_k = (code[0]-'0')*1000 + (code[1]-'0')*100 + (code[2]-'0')*10 + (code[3]-'0');
  int c_j = (j_code[0]-'0')*1000 + (j_code[1]-'0')*100 + (j_code[2]-'0')*10 + (j_code[3]-'0');

  if(c_k > c_j)
    return(1);
  else
    return(0);
}

int  perdu()
{
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("BOOM");
  lcd.setCursor(6, 1);
  lcd.print(code);
  play_sound(10, 100, 1, 1);
  lcd.clear();
  return(0);
}

int  partie_reset()
{
  digitalWrite(ledY, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("partie RESET ...");
  delay(10000);
  lcd.clear();
  return(-1);
}

int  compteur()
{
  char  carac;
  int   nb_carac = 0;
  long  depart = millis();
  long  depart_led = millis();
  long  depart_hp = millis();
  long  depart_reset;
  int   led = 0;
  int   bip = 0;
  int   tmps = timer;
  int   can_reset = 0;

  while(tmps >= 0)
  {
    //affichage
    tmps = timer - (unsigned long)(millis() - depart) / 1000;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  code:  temps:");
    lcd.setCursor(2, 1);
    lcd.print(j_code);
    //reset
    if(digitalRead(reset) == LOW && can_reset == 0)
    {
      can_reset = 1;
      depart_reset = millis();
    }
    if(digitalRead(reset) == HIGH)
      can_reset = 0;
    if(can_reset == 1 && (unsigned long)(millis() - depart_reset)/1000 > 5)
      return(partie_reset());
    //alarme
    if(tmps >= 0)
      aff_time(tmps);
    if(tmps <= 15)
    {
      digitalWrite(ledY, LOW);
      if((unsigned long)(millis() - depart_hp) >= 5)
      {
        if(bip == 0)
        {
          digitalWrite(hp,HIGH);
          bip = 1; 
        }
        else
        {
          digitalWrite(hp,LOW);
          bip = 0;
        }
        depart_hp = millis();
      }
    }
    else
    {
      if((unsigned long)(millis() - depart_led) >= 500)
      {
        if(led == 0)
        {
          digitalWrite(ledY, HIGH);
          led = 1;
        }
        else
        {
          digitalWrite(ledY, LOW);
          led = 0;
        }
        depart_led = millis();
      }
    }
    //keypad
    carac = customKeypad.getKey();
    if(nb_carac < 4 && carac)
    {
      if(carac != '*' && carac != '#')
      {
        j_code[nb_carac] = carac;
        nb_carac++;
      }
    } 
    else if (nb_carac >= 4)
    {
      if(verif_code(tmps) == 1)
          return(1);
      else
      {
        if(sup_inf() == 1)
          reset_jcode('+');
        else
          reset_jcode('-');
      }
      nb_carac = 0;
    }
    delay(150);
  }
  return(perdu());
}

int  def_tmps()
{
  char  c;
  char  t[3] = {0, 0};
  int   nb_c = 0;
  int   temps = 0;
  int   fin = 1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Definir temps :");
  while(fin == 1)
  {
    lcd.setCursor(0, 1);
    lcd.print(temps);
    c = customKeypad.getKey();
    if(c)
    {
      if(c == '#' && nb_c > 0)
      {
        t[nb_c-1] = '0';
        nb_c--;
      }
      if(c >= '0' && c <= '9' && nb_c < 2)
      {
        if(!(c == '0' && nb_c == 0))
        {
          t[nb_c] = c;
          nb_c++;
        }
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Definir temps :");
      if(nb_c == 2)
        temps = (t[0]-'0')*10 + (t[1]-'0');
      else if(nb_c == 1)
        temps = (t[0]-'0');
      else
        temps = 0;
      if(c == '*' && temps >= 5 && temps <= 60)
        fin = 0;
    }
  }
  lcd.clear();
  timer = temps * 60;
}

void depart()
{
  for(int i = 0; i < 4; i++)
  {
    play_sound(1, 100, 1, 1);
    delay(1000);
  }
  play_sound(1, 0, 250, 50);
}

void setup()
{
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  pinMode(reset, INPUT_PULLUP);
  pinMode(hp, OUTPUT);
  pinMode(ledY, OUTPUT);
  timer = 300;
  lcd.begin(16, 2);
  randomSeed(analogRead(0));
  def_tmps();
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("  press START");
  reset_jcode('*');
  if(digitalRead(button) == LOW)
  {
    lcd.clear();
    delay(1500);
    //define code  
    gen_code();
    depart();
    compteur();
  }
}

