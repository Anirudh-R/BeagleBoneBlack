/*
 ============================================================================
 Name        : main.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : MPU-6050 based tilt angle calculation.
 ============================================================================
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

/* configuration registers */
#define MPU6050_REG_POWER               0x6B
#define MPU6050_REG_ACCEL_CONFIG        0x1C
#define MPU6050_REG_GYRO_CONFIG         0x1B

/* accelerometer value registers */
#define MPU6050_REG_ACC_X_HIGH          0x3B
#define MPU6050_REG_ACC_X_LOW           0x3C
#define MPU6050_REG_ACC_Y_HIGH          0x3D
#define MPU6050_REG_ACC_Y_LOW           0x3E
#define MPU6050_REG_ACC_Z_HIGH          0x3F
#define MPU6050_REG_ACC_Z_LOW           0x40

/* gyro value registers */
#define MPU6050_REG_GYRO_X_HIGH         0x43
#define MPU6050_REG_GYRO_X_LOW          0x44
#define MPU6050_REG_GYRO_Y_HIGH         0x45
#define MPU6050_REG_GYRO_Y_LOW          0x46
#define MPU6050_REG_GYRO_Z_HIGH         0x47
#define MPU6050_REG_GYRO_Z_LOW          0x48

/* LSB sensitivities for different full scale settings */
#define ACC_FS_SENSITIVITY_0			16384
#define ACC_FS_SENSITIVITY_1		    8192
#define ACC_FS_SENSITIVITY_2		    4096
#define ACC_FS_SENSITIVITY_3		    2048

#define GYR_FS_SENSITIVITY_0			131
#define GYR_FS_SENSITIVITY_1			65.5
#define GYR_FS_SENSITIVITY_2			32.8
#define GYR_FS_SENSITIVITY_3			16.4

/* full scale options */
#define AFS_2G							0
#define AFS_4G							1
#define AFS_8G							2
#define AFS_16G							3

#define GFS_250DPS						0
#define GFS_500DPS						1
#define GFS_1000DPS						2
#define GFS_2000DPS						3

/* chosen full scale settings */
#define ACC_FS_SENSITIVITY				ACC_FS_SENSITIVITY_0
#define GYR_FS_SENSITIVITY				GYR_FS_SENSITIVITY_0
#define AFS_SEL							AFS_2G
#define GFS_SEL							GFS_250DPS

#define MPU6050_SLAVE_ADDR 				0x68

/* I2C3 controller of the SOC; SCL=P9.19, SDA=P9.20 */
#define I2C_DEVICE_FILE   				"/dev/i2c-2"


/**
  * @brief  Write a byte of data to the specified register.
  * @param  fd: I2C device file descriptor
  * @param  regaddr: register address
  * @param  val: value
  * @retval 0 if success, else -1
  */
int mpu6050_write_reg(int fd, uint8_t regaddr, uint8_t val)
{
	uint8_t buf[2];
	int ret;

	buf[0] = regaddr;
	buf[1] = val;
	ret = write(fd, buf, 2);
	if(ret < 2){
		perror("MPU-6050 write failed\n");
		return -1;
	}

	return 0;
}

/**
  * @brief  Read specified no. of bytes starting from a register.
  * @param  fd: I2C device file descriptor
  * @param  regaddr: starting register address
  * @param  pBuffer: pointer to data buffer
  * @param  len: no. of bytes to read
  * @retval 0 if success, else -1
  */
int mpu6050_read_reg(int fd, uint8_t regaddr, uint8_t *pBuffer, uint8_t len)
{
	uint8_t buf;
	int ret;

	buf = regaddr;
	ret = write(fd, &buf, 1);
	if(ret < 1){
		perror("MPU-6050 read failed\n");
		return -1;
	}

	ret = read(fd, pBuffer, len);
	if(ret < len){
		perror("MPU-6050 read failed\n");
		return -1;
	}

	return 0;
}

/**
  * @brief  Initialize MPU-6050.
  * @param  fd: I2C device file descriptor
  * @retval None
  */
void mpu6050_init(int fd)
{
	mpu6050_write_reg(fd, MPU6050_REG_POWER, 0x00);		/* disable sleep mode */
	usleep(500);

	/* set full scale values */
	mpu6050_write_reg(fd, MPU6050_REG_ACCEL_CONFIG, AFS_SEL << 3);
	usleep(500);
	mpu6050_write_reg(fd, MPU6050_REG_GYRO_CONFIG, GFS_SEL << 3);
	usleep(500);
}

/**
  * @brief  Read accelerometer values of x, y, z into a buffer.
  * @param  fd: I2C device file descriptor
  * @retval None
  */
void mpu6050_read_acc(int fd, int16_t *pBuffer)
{
	uint8_t acc_buffer[6];

    mpu6050_read_reg(fd, MPU6050_REG_ACC_X_HIGH, acc_buffer, 6);

    /* pBuffer[0] = acc x-axis value, pBuffer[1]= acc y-axis value, pBuffer[2] = acc z-axis value */
    pBuffer[0] = acc_buffer[0] << 8 | acc_buffer[1];
    pBuffer[1] = acc_buffer[2] << 8 | acc_buffer[3];
    pBuffer[2] = acc_buffer[4] << 8 | acc_buffer[5];
}

/**
  * @brief  Read gyroscope values of x, y, z into a buffer.
  * @param  fd: I2C device file descriptor
  * @retval None
  */
void mpu6050_read_gyro(int fd, int16_t *pBuffer)
{
	uint8_t gyr_buffer[6];

    mpu6050_read_reg(fd, MPU6050_REG_GYRO_X_HIGH, gyr_buffer, 6);

    /* pBuffer[0] = gyro x-axis value, pBuffer[1]= gyro y-axis value, pBuffer[2] = gyro z-axis value */
    pBuffer[0] = gyr_buffer[0] << 8 | gyr_buffer[1];
    pBuffer[1] = gyr_buffer[2] << 8 | gyr_buffer[3];
    pBuffer[2] = gyr_buffer[4] << 8 | gyr_buffer[5];
}

int main(void)
{
	int16_t acc_raw[3];
    float accx, accy, accz, anglex, angley;
	int fd;

	if((fd = open(I2C_DEVICE_FILE, O_RDWR)) < 0){
		perror("Failed to open I2C device file.\n");
		return -1;
	}

	/* set the I2C slave address using ioctl I2C_SLAVE command */
	if(ioctl(fd, I2C_SLAVE, MPU6050_SLAVE_ADDR) < 0){
		perror("Failed to set I2C slave address.\n");
		close(fd);
		return -1;
	}

	mpu6050_init(fd);

	while(1){
		mpu6050_read_acc(fd, acc_raw);

		/* convert acc raw values into 'g' values */
		accx = (float)acc_raw[0] / ACC_FS_SENSITIVITY;
		accy = (float)acc_raw[1] / ACC_FS_SENSITIVITY;
		accz = (float)acc_raw[2] / ACC_FS_SENSITIVITY;

		anglex = asin(accx / sqrt(accx*accx + accz*accz)) * 180/M_PI;
		angley = asin(accy / sqrt(accy*accy + accz*accz)) * 180/M_PI;

		printf("anglex=%0.1f   angley=%0.1f\n", anglex, angley);

		usleep(250 * 1000);
	}

	return 0;
}
