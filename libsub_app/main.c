/*
 * libusb example program to list devices on the bus
 * Copyright © 2007 Daniel Drake <dsd@gentoo.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "libusb.h"

#define VID 0x8888
#define PID 0x0088

#define edp2in 0x82
#define edp2out 0x02

typedef struct usb_pro{
	uint16_t vid;
	uint16_t pid;
	char pstr[256];
	char mstr[256];
	
}usb_pro_t;

static void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8];

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			printf("failed to get device descriptor");
			return;
		}

		printf("%04x:%04x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			printf(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				printf(".%d", path[j]);
		}
		printf("\n");
	}
}

int main(void)
{
	libusb_device **devs, *dev;
	int ret, i;
	ssize_t cnt;
	usb_pro_t usb_pro;
	struct libusb_device_handle *handle = NULL;
	libusb_context *ctx = NULL;
	

	memset((unsigned char*)&usb_pro, 0, sizeof(usb_pro_t));
	usb_pro.vid = VID;
	usb_pro.pid = PID;
	
	ret = libusb_init(&ctx);
	if (ret < 0)
		return -1;

	libusb_set_debug(ctx, 3);

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0) {
		printf("no usb dev on bus\r\n");
		return  -1;
	}

	i = 0;
	while((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		ret = libusb_get_device_descriptor(dev,&desc);
		if (ret < 0) {
			printf("failed to get device descriptor");
			goto error;
		}
		/*
		printf("%04x:%04x (bus %d, device %d)\r\n",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));
		*/
		if ((desc.idVendor == VID) && (desc.idProduct == PID)) {
			printf("bLength: 0x%04x\r\n", desc.bLength);
			printf("bDescriptorType: 0x%04x\r\n", desc.bDescriptorType);
			printf("bcdUSB: 0x%04x\r\n", desc.bcdUSB);
			printf("bDeviceClass: 0x%04x\r\n", desc.bDeviceClass);
			printf("bDeviceSubClass: 0x%04x\r\n", desc.bDeviceSubClass);
			printf("bDeviceProtocol: 0x%04x\r\n", desc.bDeviceProtocol);
			printf("bMaxPacketSize0: 0x%04x\r\n", desc.bMaxPacketSize0);
			printf("vendor id: 0x%04x\r\n", desc.idVendor);
			printf("product id: 0x%04x\r\n", desc.idProduct);
			printf("bcdDevice: 0x%04x\r\n", desc.bcdDevice);
			printf("iManufacturer: 0x%04x\r\n", desc.iManufacturer);
			printf("iProduct: 0x%04x\r\n", desc.iProduct);
			printf("iSerialNumber: 0x%04x\r\n", desc.iSerialNumber);
			printf("bNumConfigurations: 0x%04x\r\n", desc.bNumConfigurations);

		}
		
	}

	handle = libusb_open_device_with_vid_pid(ctx, VID, PID);

	if (handle == NULL) {
		printf("cant't open device\r\n");
		goto error;
	} else {
		printf("open device\r\n");
	}
	
	libusb_free_device_list(devs, 1);

	if (libusb_kernel_driver_active(handle, 0) ==1) {
		printf("kernel driver active, detach it \r\n");

		if (libusb_detach_kernel_driver(handle, 0) == 0) {
			printf("detached kernel driver\r\n");
		}
		else {
			goto error;
		}
	}

	ret = libusb_claim_interface(handle, 0);
	if (ret < 0) {
		printf("can't claim interface\r\n");
		goto error;
	} else {
		printf("claimed interface\r\n");
	}

	char data[64];
	int actual_len = 0;
	int didi = 1000;
	for (int i = 0; i< 1000; i++) {
		memset(data, 0, sizeof(data));
/*		ret = libusb_bulk_transfer(handle, edp2in, data, 64, &actual_len, 0);

		if (actual_len = 0) {
			printf("received nothing\r\n");
		} else {
			printf("bulk transfer: %s\r\n", data);
		}

		usleep(200000);
*/
		char *str = "am host";
		//strncpy(data, str, strlen(str));
        sprintf(data, "am host %d\r\n", i);
        printf("%s", data);
		ret = libusb_bulk_transfer(handle, edp2out, data, strlen(data), &actual_len, 0);

		if (actual_len = 0) {
			printf("received nothing\r\n");
		} else {
			printf("send data: %s\r\n", data);
		}

		usleep(200000);
        
	}

	libusb_close(handle);

error:

	printf("free device list\r\n");
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return 0;
}

