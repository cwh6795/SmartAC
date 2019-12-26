#include <pm2008_i2c.h>

PM2008_I2C pm2008_i2c;

void setup() {
  pm2008_i2c.begin();
  Serial.begin(9600);
  pm2008_i2c.command();
  delay(1000);
}

void loop() {
  uint8_t ret = pm2008_i2c.read();
  if (ret == 0) {
    Serial.print("PM 1.0 (GRIMM) : ");
    Serial.println(pm2008_i2c.pm1p0_grimm); // grimm >> tsi
    Serial.print("PM 2.5 (GRIMM) : : ");
    Serial.println(pm2008_i2c.pm2p5_grimm);
    Serial.print("PM 10 (GRIMM) : : ");
    Serial.println(pm2008_i2c.pm10_grimm);
  }
  else {
    Serial.print("READ ERROR!!");
  }
  delay(1000);
}
