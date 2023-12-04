#INCLUDE <18f4550.h>
#FUSES NOPROTECT, NOWDT, NOLVP, XT, PLL1, CPUDIV1
#USE delay(clock = 4M)
#USE rs232(rcv=pin_c7,xmit=pin_c6,baud=9600,bits=8,parity=n,stream = GSM)
#USE rs232(rcv=pin_b3, xmit=pin_b4,baud=9600,bits=8,parity=n,stream = GPS)
#USE rs232(rcv=pin_b5, xmit=pin_b6,baud=9600,bits=8,parity=n,stream = TTL)
#USE I2C(MASTER, SDA = PIN_B0, SCL = PIN_B1,SLOW)

#INCLUDE <stdlib.h>
#INCLUDE <MPU6050.c>

#BYTE INTCON = 0xFF2
#BIT IPEN = 0xFD0.7
#BIT RCIE = 0xF9D.5

#BIT TRISB7 = 0xF93.7
#BIT B7 = 0xF81.7

char dat = 0, msj[60];
int count = 0, band2 = 0;
float Ay = 0, Ax = 0, Az = 0, pA = 0;

int i;
char ch;
char gps_full[100];
// COORDENADAS LATITUD
char gps_lat1[2];
char gps_mlat1[2];
char gps_slat1[4];
char gps_slat1x[5];
char lat1[13];
float slat1;
// COORDENADAS LONGITUD
char gps_long[3];
char gps_mlong[2];
char gps_slong[4];
char gps_slongx[5];
float slong;
char clong[14];
// ORIENTACIÓN
char o_long;

#int_RDA
void rda_isr(){
   if(kbhit(GSM)){
      dat = fgetc(GSM);
      if(dat == '+'){
         count = 0;
      }
      msj[count++] = dat;
   }
}

void buffercl()
{
   for(i=0;i<100;i++)
   {
      gps_full[i]=0;
   }
   for(i=0;i<2;i++)
   {
      gps_lat1[i]=0;
   }
   for(i=0;i<3;i++)
   {
      gps_long[i]=0;
   }
   for(i=0;i<2;i++)
   {
      gps_mlong[i]=0;
   }
   for(i=0;i<5;i++)
   {
      gps_slongx[i]=0;
   }
   ///////////////////
   for(i=0;i<2;i++)
   {
      gps_mlat1[i]=0;
   }
   for(i=0;i<4;i++)
   {
      gps_slat1[i]=0;
   }
   for(i=0;i<5;i++)
   {
      gps_slat1x[i]=0;
   }
   for(i=0;i<4;i++)
   {
      gps_slong[i]=0;
   }
   ///////////////////
   for(i=0;i<13;i++) // SE CAMBIÓ 12 POR 13
   {
      lat1[i]=0;
   }
   for(i=0;i<14;i++)
   {
      clong[i]=0;
   }
}

void clean(){
   for(int i=0;i<60;i++){
      msj[i] = 0;
      delay_ms(50);
   }
   dat = 0;
   count = 0;
}

void coord_lat1() // COORDENADA MENOR A 100
{
   for(i=0;i<2;i++)
   {
      lat1[i]=gps_lat1[i];
   }
   lat1[2]='G'; // 167 y 248 ORIGINALMENTE
   lat1[3]=gps_mlat1[0];
   lat1[4]=gps_mlat1[1];
   lat1[5]=39; // "'"
   lat1[6]=gps_slat1x[0];
   lat1[7]=gps_slat1x[1];
   lat1[8]=gps_slat1x[2];
   lat1[9]=gps_slat1x[3];
   lat1[10]=gps_slat1x[4];
   lat1[11]=34;
   lat1[12]=gps_full[29]; // ORIENTACIÓN LATITUD
}

void coord_long() // COORDENADA MENOR A 100
{
   for(i=0;i<3;i++)
   {
      clong[i]=gps_long[i];
   }
   clong[3]='G'; // 167 ORIGINALMENTE
   clong[4]=gps_mlong[0];
   clong[5]=gps_mlong[1];
   clong[6]=39; // "'"
   clong[7]=gps_slongx[0];
   clong[8]=gps_slongx[1];
   clong[9]=gps_slongx[2];
   clong[10]=gps_slongx[3];
   clong[11]=gps_slongx[4];
   clong[12]=34;
   clong[13]=o_long;
}

void conv_seg_str1() // CONVERTIR SEGUNDOS A STRING (MENOR A 100 - LATITUD)
{
   int16 u=0,d=0,c=0,m=0,num=0;
   num=slat1*100;
   u=num%10;
   d=num/10;
   d=d%10;
   c=num%1000;
   c=c/100;
   m=num/1000;
   switch(u)
   {
      case 0:
         gps_slat1x[4]='0';
         break;
      case 1:
         gps_slat1x[4]='1';
         break;
      case 2:
         gps_slat1x[4]='2';
         break;
      case 3:
         gps_slat1x[4]='3';
         break;
      case 4:
         gps_slat1x[4]='4';
         break;
      case 5:
         gps_slat1x[4]='5';
         break;
      case 6:
         gps_slat1x[4]='6';
         break;
      case 7:
         gps_slat1x[4]='7';
         break;
      case 8:
         gps_slat1x[4]='8';
         break;
      case 9:
         gps_slat1x[4]='9';
         break;     
      default:
         gps_slat1x[4]='W';
         break;         
   }
   switch(d)
   {
      case 0:
         gps_slat1x[3]='0';
         break;
      case 1:
         gps_slat1x[3]='1';
         break;
      case 2:
         gps_slat1x[3]='2';
         break;
      case 3:
         gps_slat1x[3]='3';
         break;
      case 4:
         gps_slat1x[3]='4';
         break;
      case 5:
         gps_slat1x[3]='5';
         break;
      case 6:
         gps_slat1x[3]='6';
         break;
      case 7:
         gps_slat1x[3]='7';
         break;
      case 8:
         gps_slat1x[3]='8';
         break;
      case 9:
         gps_slat1x[3]='9';
         break;     
      default:
         gps_slat1x[3]='X';
         break;         
   }
   gps_slat1x[2]='.';
   switch(c)
   {
      case 0:
         gps_slat1x[1]='0';
         break;
      case 1:
         gps_slat1x[1]='1';
         break;
      case 2:
         gps_slat1x[1]='2';
         break;
      case 3:
         gps_slat1x[1]='3';
         break;
      case 4:
         gps_slat1x[1]='4';
         break;
      case 5:
         gps_slat1x[1]='5';
         break;
      case 6:
         gps_slat1x[1]='6';
         break;
      case 7:
         gps_slat1x[1]='7';
         break;
      case 8:
         gps_slat1x[1]='8';
         break;
      case 9:
         gps_slat1x[1]='9';
         break;     
      default:
         gps_slat1x[1]='Y';
         break;         
   }
   switch(m)
   {
      case 0:
         gps_slat1x[0]='0';
         break;
      case 1:
         gps_slat1x[0]='1';
         break;
      case 2:
         gps_slat1x[0]='2';
         break;
      case 3:
         gps_slat1x[0]='3';
         break;
      case 4:
         gps_slat1x[0]='4';
         break;
      case 5:
         gps_slat1x[0]='5';
         break;
      case 6:
         gps_slat1x[0]='6';
         break;
      case 7:
         gps_slat1x[0]='7';
         break;
      case 8:
         gps_slat1x[0]='8';
         break;
      case 9:
         gps_slat1x[0]='9';
         break;     
      default:
         gps_slat1x[0]='Z';
         break;         
   }
}

void conv_seg_str3() // CONVERTIR SEGUNDOS A STRING (MAYOR A 100 - LONGITUD)
{
   int16 u=0,d=0,c=0,m=0,num=0;
   num=slong*100;
   u=num%10;
   d=num/10;
   d=d%10;
   c=num%1000;
   c=c/100;
   m=num/1000;
   switch(u)
   {
      case 0:
         gps_slongx[4]='0';
         break;
      case 1:
         gps_slongx[4]='1';
         break;
      case 2:
         gps_slongx[4]='2';
         break;
      case 3:
         gps_slongx[4]='3';
         break;
      case 4:
         gps_slongx[4]='4';
         break;
      case 5:
         gps_slongx[4]='5';
         break;
      case 6:
         gps_slongx[4]='6';
         break;
      case 7:
         gps_slongx[4]='7';
         break;
      case 8:
         gps_slongx[4]='8';
         break;
      case 9:
         gps_slongx[4]='9';
         break;     
      default:
         gps_slongx[4]='W';
         break;         
   }
   switch(d)
   {
      case 0:
         gps_slongx[3]='0';
         break;
      case 1:
         gps_slongx[3]='1';
         break;
      case 2:
         gps_slongx[3]='2';
         break;
      case 3:
         gps_slongx[3]='3';
         break;
      case 4:
         gps_slongx[3]='4';
         break;
      case 5:
         gps_slongx[3]='5';
         break;
      case 6:
         gps_slongx[3]='6';
         break;
      case 7:
         gps_slongx[3]='7';
         break;
      case 8:
         gps_slongx[3]='8';
         break;
      case 9:
         gps_slongx[3]='9';
         break;     
      default:
         gps_slongx[3]='X';
         break;         
   }
   gps_slongx[2]='.';
   switch(c)
   {
      case 0:
         gps_slongx[1]='0';
         break;
      case 1:
         gps_slongx[1]='1';
         break;
      case 2:
         gps_slongx[1]='2';
         break;
      case 3:
         gps_slongx[1]='3';
         break;
      case 4:
         gps_slongx[1]='4';
         break;
      case 5:
         gps_slongx[1]='5';
         break;
      case 6:
         gps_slongx[1]='6';
         break;
      case 7:
         gps_slongx[1]='7';
         break;
      case 8:
         gps_slongx[1]='8';
         break;
      case 9:
         gps_slongx[1]='9';
         break;     
      default:
         gps_slongx[1]='Y';
         break;         
   }
   switch(m)
   {
      case 0:
         gps_slongx[0]='0';
         break;
      case 1:
         gps_slongx[0]='1';
         break;
      case 2:
         gps_slongx[0]='2';
         break;
      case 3:
         gps_slongx[0]='3';
         break;
      case 4:
         gps_slongx[0]='4';
         break;
      case 5:
         gps_slongx[0]='5';
         break;
      case 6:
         gps_slongx[0]='6';
         break;
      case 7:
         gps_slongx[0]='7';
         break;
      case 8:
         gps_slongx[0]='8';
         break;
      case 9:
         gps_slongx[0]='9';
         break;     
      default:
         gps_slongx[0]='Z';
         break;         
   }
}

void conv_coord1() // COORDENADA MENOR A 100
{
   for(i=0;i<2;i++)
   {
      ch=fgetc(GPS);
      gps_lat1[i]=gps_full[i+18];
   }
   for(i=0;i<2;i++)
   {
      ch=fgetc(GPS);
      gps_mlat1[i]=gps_full[i+20];
   }
   for(i=0;i<4;i++)
   {
      ch=fgetc(GPS);
      gps_slat1[i]=gps_full[i+23];
   }
   ch=fgetc(GPS);
   slat1=atof(gps_slat1);
   ch=fgetc(GPS);
   slat1=((slat1)*(60))/(10000);
   ch=fgetc(GPS);
   for(i=0;i<3;i++)
   {
      ch=fgetc(GPS);
      gps_long[i]=gps_full[i+31];
   }
   for(i=0;i<2;i++)
   {
      ch=fgetc(GPS);
      gps_mlong[i]=gps_full[i+34];
   }
   for(i=0;i<4;i++)
   {
      ch=fgetc(GPS);
      gps_slong[i]=gps_full[i+37];
   }
   ch=fgetc(GPS);
   slong=atof(gps_slong);
   ch=fgetc(GPS);
   slong=((slong)*(60))/(10000);
   ch=fgetc(GPS);
}

void rcvgps(){
   ch=fgetc(GPS);
      if(ch=='$')
      {
         for(i=0;i<5;i++)
         {
            gps_full[i]=fgetc(GPS);
         }
         if(gps_full[0]=='G' && gps_full[1]=='P' && gps_full[2]=='R' && gps_full[3]=='M' && gps_full[4]=='C')
         {           
            for(i=5;i<44;i++)
            {  
               gps_full[i]=fgetc(GPS);
            }
            // IMPRIMIR LATITUD
            conv_coord1();
            conv_seg_str1();
            coord_lat1();
            ch=fgetc(GPS);
            // IMPRIMIR LONGITUD
            conv_seg_str3();
            o_long=gps_full[43];
            ch=fgetc(GPS);
            coord_long();
            ch=fgetc(GPS);
          }
      }
}

void sendsms(){

   B7 = 1;
   delay_ms(2000);
   B7 = 0;

   fprintf(GSM, "AT+CMGF=1\r\n");
   delay_ms(500);
         
   fprintf(GSM, "AT+CMGS=\"+524545454544545454\"\r\n");
   delay_ms(500);
   
   fprintf(GSM, "Coordenadas:\n\r");
   //fprintf(TTL, "Coordenadas:\n\r");
   fprintf(GSM, "Latitud: ");
   //fprintf(TTL, "Latitud: ");
   for(int i=0;i<13;i++){
      fprintf(GSM, "%c", lat1[i]);
      //fprintf(TTL, "%c", lat1[i]);
   }
   fprintf(GSM, "\n\rLongitud: ");
   //fprintf(TTL, "\n\rLongitud: ");
   for(int j=0;j<14;j++){
      fprintf(GSM, "%c", clong[j]);
      //fprintf(TTL, "%c", clong[j]);
   }
   fprintf(GSM, "\n\r");
   //fprintf(TTL, "\n\r");
   putchar(0x1a,GSM);   // equivalente a CTRL + 'Z'
   delay_ms(1000);
   
   B7 = 1;
   delay_ms(2000);
   B7 = 0;
}

void rcvsms(){
   
   fprintf(GSM,"AT+CMGD=1\n\r");
   delay_ms(1000);

   for(int i=0;i<60;i++){
      if(msj[i] == 'S' && msj[i+1] == 'T'){
         band2 = 0;
         clean();
      }
   }
}

void main(){
   delay_ms(5000);
   
   TRISB7 = 0;
   B7 = 0;
   IPEN = 0;
   INTCON = 0b11000000;
   RCIE = 1;
   
   MPU6050_init();
   
   clean();
   buffercl();
   
   fprintf(GSM,"ATE0\n\r");
   delay_ms(1000);
   fprintf(GSM,"AT+CPMS=\"SM\",\"SM\"\n\r");
   delay_ms(1000);
   fprintf(GSM,"AT+CNMI=2,2,0,0,0\n\r");
   delay_ms(1000);
   fprintf(GSM,"AT+CMGF=1\n\r");
   delay_ms(1000);
   
   B7 = 1;
   delay_ms(2000);
   B7 = 0;
   
   while(TRUE){
      for(int p=0;p<10;p++){
         Ax += MPU6050_get_Ax();
         Ay += MPU6050_get_Ay();
         Az += MPU6050_get_Az();
      }
      
      Ax = Ax/10;
      Ay = Ay/10;
      Az = Az/10;
      
      pA = (Ax+Ay+Az)/3;
      
      if(pA > 0.5 || pA < 0.3) band2 = 1; 
      
      while(band2 == 1){
         rcvgps();
         delay_ms(200);
         rcvsms();
         delay_ms(200);
         if(band2 == 0) break;
         sendsms();
         delay_ms(8000);
         buffercl();
         delay_ms(200);
      }//Envio de mensajes cada 15s aprox
      
      /*
      fprintf(TTL, "Ax:%0.2f\n\r", Ax);   // Imprime Ax
      fprintf(TTL, "Ay:%0.2f\n\r", Ay);   // Imprime Ax
      fprintf(TTL, "Az:%0.2f\n\r", Az);   // Imprime Ax
      fprintf(TTL, "pA:%0.2f\n\r", pA);   // Imprime Ax
      delay_ms(2000);
      */
   }
}
