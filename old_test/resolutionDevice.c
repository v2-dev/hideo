#include "resolutionDevice.h"


int get_height(wurfl_device_handle hdevice){

	if (hdevice) return  wurfl_device_get_capability_as_int(hdevice, "resolution_height");
	else return DEFAULT_HEIGHT;
}

int get_width(wurfl_device_handle hdevice){

	if (hdevice) return  wurfl_device_get_capability_as_int(hdevice, "resolution_width");
	else return DEFAULT_WIDTH;
}

wurfl_device_handle get_device(wurfl_handle hwurfl, char *userAgent){

	wurfl_device_handle hdevice = wurfl_lookup_useragent(hwurfl, userAgent);
	return hdevice;
}

wurfl_handle get_wurfldb(char *root){

	wurfl_handle hwurfl;
	wurfl_error error;

	// Create wurfl handler
	hwurfl = wurfl_create();

	// Define wurfl db
	error = wurfl_set_root(hwurfl, root);
	if (error != WURFL_OK) {
		fprintf(stderr, "%s\n", wurfl_get_error_message(hwurfl));
		return NULL;
	}

	// Loads wurfl db into memory
	error = wurfl_load(hwurfl);
	if (error != WURFL_OK) {
		fprintf(stderr, "%s\n", wurfl_get_error_message(hwurfl));
		return NULL;
	}

	return hwurfl;
}

void wurflpdt(wurfl_handle wurfldb, char *uastring)
{
		wurfl_device_handle hdevice = get_device(wurfldb, uastring);

		if (hdevice == 0) {

			printf("Device not found\n");
		}

		else {
			printf("%d\tx\t", get_height(hdevice));
			printf("%d\n", get_width(hdevice));
		}
}

void wurflrdt(wurfl_handle wurfldb, char *uastring, int* heightv, int* widthv)
{
		wurfl_device_handle hdevice = get_device(wurfldb, uastring);
		*heightv = get_height(hdevice);
		*widthv = get_width(hdevice);
		wurfl_device_destroy(hdevice);
}
