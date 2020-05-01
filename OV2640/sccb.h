#ifndef SCCB_H
#define SCCB_H

void sccb_init(void);
void sccb_write(uint8_t id, uint8_t sub, uint8_t data);
uint8_t sccb_read(uint8_t id, uint8_t sub);

#endif