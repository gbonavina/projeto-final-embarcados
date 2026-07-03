sbit LCD_RS at RE2_bit;   // PINO 2 DO PORTD INTERLIGADO AO RS DO DISPLAY
sbit LCD_EN at RE1_bit;   // PINO 3 DO PORTD INTERLIGADO AO EN DO DISPLAY
sbit LCD_D7 at RD7_bit;  // PINO 7 DO PORTD INTERLIGADO AO D7 DO DISPLAY
sbit LCD_D6 at RD6_bit;  // PINO 6 DO PORTD INTERLIGADO AO D6 DO DISPLAY
sbit LCD_D5 at RD5_bit;  // PINO 5 DO PORTD INTERLIGADO AO D5 DO DISPLAY
sbit LCD_D4 at RD4_bit;  // PINO 4 DO PORTD INTERLIGADO AO D4 DO DISPLAY

// Selecionando direcao de fluxo de dados dos pinos utilizados para a comunicacao com display LCD
sbit LCD_RS_Direction at TRISE2_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 2 DO PORTD
sbit LCD_EN_Direction at TRISE1_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 3 DO PORTD
sbit LCD_D7_Direction at TRISD7_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 7 DO PORTD
sbit LCD_D6_Direction at TRISD6_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 6 DO PORTD
sbit LCD_D5_Direction at TRISD5_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 5 DO PORTD
sbit LCD_D4_Direction at TRISD4_bit;  // SETA DIRECAO DO FLUXO DE DADOS DO PINO 4 DO PORTD

char ucRead;
int fator;
int totalCredits = 0;
int peso;
int temp_adc = 0;
int uiValor;

char past_RB0 = 0;
char past_RB1 = 0;
char past_RB2 = 0;
char past_RB3 = 0;
char past_RB4 = 0;

unsigned char ucMask[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
unsigned char digit_values[4] = {0, 0, 0, 0};
unsigned char current_digit = 0;


char peso_str[6];

void main() {
     TRISB.RB0 = 1;
     TRISB.RB1 = 1;
     TRISB.RB2 = 1;
     TRISB.RB3 = 1;
     TRISB.RB4 = 1;

     TRISD = 0;
     ADCON1 = 0x0E;
     PORTA = 255;
     TRISA = 00000001;

     PWM1_Init(5000);
     totalCredits = 0;

     Lcd_Init();
     Lcd_Cmd(_LCD_CURSOR_OFF);
     Lcd_Cmd(_LCD_CLEAR);

     UART1_Init(9600);

     while(1) {
         if (UART1_Data_Ready()) {
            ucRead = UART1_Read();
            Delay_ms(50);

         }
         if (ucRead == 0) {
            Lcd_Cmd(_LCD_CLEAR);

            PWM1_Set_Duty(255);
            PWM1_Start();

            Delay_ms(3000);

            PWM1_Stop();

            ucRead = 0xFF;
         }
         if (ucRead == 1) {
            lcd_out(1, 1, "User: E6B099AC");
            // printar os creditos;

            temp_adc = Adc_Read(0);
            Delay_10us;
            peso = temp_adc * 0.01467; // (10/1023)
            wordToStr(peso, peso_str);
            lcd_out(2, 4, peso_str);
            lcd_out(2, 1, "Peso: ");
            lcd_out(2, 10, "Kg");
            
            // Mostra valor da unidade:
            PORTD = ucMask[uiValor%10];        // Pega resto da divisao por 10 e coloca o
      					 // valor no PORTD.
            PORTA.RA5 = 1;                     // Liga o transistor associado ao display 1.
            Delay_ms(2);                       // Delay para escrita no display.
            PORTA.RA5 = 0;                     // Desliga o transistor associado ao display 1.
            uiValor/=10;                       // Divide variavel por 10 para definir a dezena.
            // Mostra valor da dezena:
            PORTD = ucMask[uiValor%10];
            PORTA.RA4 = 1;
            Delay_ms(2);
            PORTA.RA4 = 0;
            uiValor/=10;
            // Mostra valor da centena:
            PORTD = ucMask[uiValor%10];
            PORTA.RA3 = 1;
            Delay_ms(2);
            PORTA.RA3 = 0;
            uiValor/=10;
            // Mostra valor do milhar:
            PORTD = ucMask[uiValor%10];
            PORTA.RA2 = 1;
            Delay_ms(2);
            PORTA.RA2 = 0;

            // VIDRO
            if (PORTB.RB0 == 0 && past_RB0 == 0) {
               Delay_ms(20);
               if (PORTB.RB0 == 0) {
                  fator = 2;
                  totalCredits += fator * peso;
                  past_RB0 = 1;
               }
            }
            if (PORTB.RB0 == 1) past_RB0 = 0;

            // PAPEL
            if (PORTB.RB1 == 0 && past_RB1 == 0) {
               Delay_ms(20);
               if (PORTB.RB1 == 0) {
                  fator = 1;
                  totalCredits += fator * peso;
                  past_RB1 = 1;
               }
            }
            if (PORTB.RB1 == 1) past_RB1 = 0;

            // ORGANICO
            if (PORTB.RB2 == 0 && past_RB2 == 0) {
               Delay_ms(20);
               if (PORTB.RB2 == 0) {
                  fator = 2;
                  totalCredits += fator * peso;
                  past_RB2 = 1;
               }
            }
            if (PORTB.RB2 == 1) past_RB2 = 0;

            // METAL
            if (PORTB.RB3 == 0 && past_RB3 == 0) {
               Delay_ms(20);
               if (PORTB.RB3 == 0) {
                  fator = 3;
                  totalCredits += fator * peso;
                  past_RB3 = 1;
               }
            }
            if (PORTB.RB3 == 1) past_RB3 = 0;

            // PLASTICO
            if (PORTB.RB4 == 0 && past_RB4 == 0) {
               Delay_ms(20);
               if (PORTB.RB4 == 0) {
                  fator = 4;
                  totalCredits += fator * peso;
                  past_RB4 = 1;
               }
            }
            if (PORTB.RB4 == 1) past_RB4 = 0;

            uiValor = totalCredits;
         }

     }
}