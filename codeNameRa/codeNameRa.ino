#include <MFRC522.h>
#include <SPI.h>
#include <SD.h>
#define PINO_RST 9
#define PINO_SDA 8


MFRC522 rfid(PINO_SDA, PINO_RST);
File dados;
void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  SPI.begin();      // Inicia  SPI bus
  rfid.PCD_Init();
  if (!SD.begin(4)) {
    Serial.println("Falha na inicialização");
    while (1);
  }
  Serial.println("Aproxime a sua tag...");
  Serial.println();
}
int pos1=0;
int pos = 0;
int cont = 0;
int cont3 = 0;
int cont4 = 0;
String teste;
String nomePalestra = " Palestra2  ";
void loop()
{
  //Procura nova tag
  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  // Seleciona uma tag
  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }

  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo = "";

  for (byte i = 0; i < rfid.uid.size; i++)
  {
    conteudo.concat(String(rfid.uid.uidByte[i] < HEX ? " 0" : " "));
    conteudo.concat(String(rfid.uid.uidByte[i], HEX));
  }

  Serial.println(conteudo);
  dados = SD.open("registro.txt");
  if (dados) {
    String val;
    String temp;
    String conteudoLido; //DELCALRAR POS AQUI
    pos = 0;
    while (dados.available()) {
      val.concat(char(dados.read()));
      for (int i = 0; i < val.length(); i++) {
        String temp = val.substring(i, i + 1);
        if (temp.equalsIgnoreCase(";")) {
          conteudoLido = val.substring(0, i);
          val = "";
          int n2;
          n2 = conteudoLido.length();
          pos += n2 + 35;
          dados.seek(pos);
          Serial.println("TAG Registrada: " + conteudoLido + " TAG Solicitada: " + conteudo);
          if (conteudoLido == nomePalestra) {
            cont3++;
            delay(10);
            Serial.print("cont3:");
            Serial.println(cont3);
          }
          if (conteudoLido == conteudo) {
            pos = 0;
            cont++;
            break;
          }
        }
      }
      if (cont == 1) {
        break;
      }
    }
    dados.close();
  }
  else {
    Serial.println("Problema na conexão!");
    dados.close();
  }


  delay(1000);
  if (cont == 0) {
    cont3 = 0;
    delay(10);
    dados = SD.open("registro.txt", FILE_WRITE);
    String nomeSala;
    String nome;
    String sala;
    Serial.println("");
    Serial.println("Insira seu nome e sala");
    while (Serial.available() == 0) {
      Serial.print(".");
      delay(500);
    }
    if (Serial.available() > 1) {
      nomeSala = Serial.readString();
    }
    int cont2 = 0;
    for (int i = 0; i < nomeSala.length(); i++) {
      String temp = nomeSala.substring(i, i + 1);
      if ((temp.equalsIgnoreCase("1")) || (temp.equalsIgnoreCase("2")) || (temp.equalsIgnoreCase("3")) || (temp.equalsIgnoreCase("4"))) {
        nome = nomeSala.substring(0, i);
        sala = nomeSala.substring(i, nomeSala.length() - 1);
        cont2++;
        break;
      }
    }
    if (cont2 == 0) {
      nome = nomeSala.substring(0, nomeSala.length() - 1);
    }
    int n1 = nome.length() + sala.length() + 2;
    String temp;
    for (int i = 0; i < 32 - n1; i++) {
      temp.concat(" ");
    }
    temp.concat(".");
    Serial.println("\nGravado!\nTag:" + conteudo + "\nNome: " + nome + "\nSala: " + sala + temp);
    dados.print(conteudo);
    dados.print(";");
    dados.print(nome);
    dados.print(";");
    dados.println(sala + temp);
    dados.close();
    Serial.println("Done!\n--------------------");
    delay(50);
    Serial.println("\nAproxime o cartão!");
  } //fim se não estiver cadastrado



  if (cont == 1) { //está cadastrado já
    cont = 0;
    if (cont3 == 0) { //o cara está cadastrado em um antiga, não precisa do nome somente do do seo codigo hex
      Serial.println("Somente iremos usar seu codigo hexadecimal, não necessita de nome!");

      dados = SD.open("registro.txt");
      if (dados) {
        String val;
        String temp;
        String conteudoLido; //DELCALRAR POS AQUI
        pos1 = 0;
        while (dados.available()) {
          val.concat(char(dados.read()));
          for (int i = 0; i < val.length(); i++) {
            String temp = val.substring(i, i + 1);
            if (temp.equalsIgnoreCase(";")) {
              conteudoLido = val.substring(0, i);
              val = "";
              int n2;
              n2 = conteudoLido.length();
              pos1 += n2 + 35;
              dados.seek(pos1);
              Serial.println("TAG Registrada: " + conteudoLido + " TAG Solicitada: " + conteudo);
              if (conteudoLido == conteudo) {
                cont4++;
                if (cont4 == 2) {
                  pos1 = 0;
                  break;
                }
              }
            }
          }
          if (cont4 == 2) {
            break;
          }
        }
        dados.close();
      }
      else {
        Serial.println("Problema na conexão!");
        dados.close();
      }

      if (cont4 == 1) { //só está cadastrado na base, precisa cadastrar na palestra atual
        dados = SD.open("registro.txt", FILE_WRITE);
        if (dados) {
          int n1 = conteudo.length();
          String temp;
          for (int i = 0 ; i < 31; i++ ) {
            temp.concat(" ");
          }
          temp.concat(".");
          dados.print(conteudo);
          dados.print(";");
          dados.println(temp);
          dados.close();
          delay(50);
          Serial.println("Presente na palestra atual");
        }
        else {
          Serial.println("Conexão falhou");
          dados.close();
        }
      }

      if (cont4 == 2) {
        Serial.println("Aluno já cadastrado na palestra atual!");
      }
      cont4 = 0;
    }

    if (cont3 == 1) { //o cara está cadastrado na nova, então não precis mais;
        Serial.println("Aluno já cadastrado na palestra atual!");
    }
    cont3 = 0;
    Serial.println("\nAproxime o cartão!");
  }
}
