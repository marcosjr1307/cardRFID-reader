//Including dependencies
#include <MFRC522.h>  //Remember: You need to install the library -> 'library/rfid-master.zip'
#include <SPI.h>
#include <SD.h>

//Defining RST and SDA pins of the sensor RFID
#define PINO_RST 9
#define PINO_SDA 8


MFRC522 rfid(PINO_SDA, PINO_RST); //Instantiating the sensor RFID with the pins
File dataSD; //Instantiating the SD


//-----------IMPORTANT-----------//
//Only read this code if you have read the readme.me file,
//so you can better understand the program logic.


void setup()
{
  Serial.begin(9600); //Starting the monitor serial
  while (!Serial) {
    ;
  }

  SPI.begin();      //Starting the SPI connection
  rfid.PCD_Init(); //Initializing the RFID sensor
  //Initializing the SD
  if (!SD.begin(4)) {
    Serial.println("Initializing failed");
    while (1);
  }
  Serial.println("Approximately the card...");
  Serial.println();
}


void loop()
{
  //Routine to get the card's hexadecimal code
  //Find a new tag
  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  //Select a new tag
  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }

  //Print the tag's UID
  Serial.print("UID of the tag :");
  String content = "";

  for (byte i = 0; i < rfid.uid.size; i++)
  {
    content.concat(String(rfid.uid.uidByte[i] < HEX ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }

  Serial.println(content);

  //Routine to write some data in the document (file.txt)
  dataSD = SD.open("file.txt", FILE_WRITE); //IMPORTANT: THE FILE NAME CANNOT BE LONGER THAN 8 LETTERS
  if (dataSD) {
    dataSD.print("UID: "); 
    dataSD.print(fileContent);
    dataSD.close();
    Serial.println("Done!\n--------------------\n\n");
  }else{
    Serial.println("Conection failed");
    dataSD.close();
  }

  delay(1000);
  
  //Routine to read something in the document (file.txt)
  dataSD = SD.open("file.txt"); //IMPORTANT: THE FILE NAME CANNOT BE LONGER THAN 8 LETTERS
  if (dataSD) {
    String fileContent;
    int index = 0; //Initializing an index
    //It's important to say that this following method can help you navigate arround your file.txt.
    //Each character in your file represents a point(index), which start in the zero(index).
    //Your file seems like a simple array, and with the .seek() method you move the index.
    //Example: (Insinde your file.txt right at the beginning there is this phrase: 'Arduino');
    //If you use this dataSD.seek(3), the index will going to Ard<index>uino.
    //And the method dataSD.read() reads from the index to the end of the line and returns the value
    //String test = char(dataSD.read()); The method returns the value in the ASCII table, so you need to parse them. 
    dataSD.seek(index);
    fileContent = String(dataSD.read());
    Serial.println("Data: " + fileContent);
    dataSD.close(); //You always need to remeber to close the dataSD before starting another conecction with the documment
  }
  else {
    Serial.println("Conection failed");
    dataSD.close();
  }
  Serial.println("Approximately the card...");
}
