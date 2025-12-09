#include <PZEM004Tv30.h>
#include <HardwareSerial.h> 
#define PZEM1_RX_PIN 26 
#define PZEM1_TX_PIN 27 
#define PZEM2_RX_PIN 16 
#define PZEM2_TX_PIN 17 
#define PZEM3_RX_PIN 18 
#define PZEM3_TX_PIN 19 


HardwareSerial SerialPZEM1(1);
HardwareSerial SerialPZEM2(2); 
HardwareSerial SerialPZEM3(0); 
PZEM004Tv30 pzem1(SerialPZEM1, PZEM1_RX_PIN, PZEM1_TX_PIN);
PZEM004Tv30 pzem2(SerialPZEM2, PZEM2_RX_PIN, PZEM2_TX_PIN);
PZEM004Tv30 pzem3(SerialPZEM3, PZEM3_RX_PIN, PZEM3_TX_PIN);

const float CORRECTION_FACTOR = 2.0;

void setup() {
  Serial.begin(115200);
  Serial.println("Monitoring Energi Tiga Fasa Dimulai...");
  SerialPZEM1.begin(9600, SERIAL_8N1, PZEM1_RX_PIN, PZEM1_TX_PIN);
  SerialPZEM2.begin(9600, SERIAL_8N1, PZEM2_RX_PIN, PZEM2_TX_PIN);
  SerialPZEM3.begin(9600, SERIAL_8N1, PZEM3_RX_PIN, PZEM3_TX_PIN);

  delay(2000);
}
void loop() {
  get_pzem_data("FASA 1", pzem1);
  delay(100); // Jeda singkat
  get_pzem_data("FASA 2", pzem2);
  delay(100); // Jeda singkat
  get_pzem_data("FASA 3", pzem3);
  delay(100); // Jeda singkat
  calculate_and_print_total();

  Serial.println("----------------------------------------");
  delay(5000); 
}

float total_power = 0.0;
float total_energy = 0.0;

void get_pzem_data(const char* name, PZEM004Tv30& pzem) {
  float voltage = pzem.voltage();
  float current = pzem.current() * CORRECTION_FACTOR; // Terapkan faktor skala CT
  float power = pzem.power() * CORRECTION_FACTOR;     // Terapkan faktor skala CT
  float energy = pzem.energy() * CORRECTION_FACTOR;   // Terapkan faktor skala CT
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  if (isnan(voltage)) {
    Serial.printf("%s: Gagal membaca data dari PZEM! Cek koneksi.\n", name);
    return;
  }

  Serial.printf("\n--- %s ---\n", name);
  Serial.printf("Tegangan (V): %.1f\n", voltage);
  Serial.printf("Arus (A): %.3f\n", current);
  Serial.printf("Daya Aktif (W): %.2f\n", power);
  Serial.printf("Energi (kWh): %.4f\n", energy);
  Serial.printf("Frekuensi (Hz): %.1f\n", frequency);
  Serial.printf("Power Factor (PF): %.2f\n", pf);

  total_power += power;
  total_energy += energy; 
}


void calculate_and_print_total() {
  Serial.println("\n*** REKAPITULASI TIGA FASA ***");
  Serial.printf("Total Daya Aktif (W): %.2f\n", total_power);
  total_power = 0.0; 
  
  // Catatan: Total energi yang ditampilkan di sini adalah penjumlahan kumulatif dari 3 PZEM.
  Serial.printf("Total Energi Kumulatif (kWh): %.4f\n", total_energy);
  // total_energy tidak direset karena ini adalah pembacaan kumulatif.
}
