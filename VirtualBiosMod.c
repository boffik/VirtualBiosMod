/*
        This file is part of VirtualBiosMod, a virtual bios setup interface
        Copyright (C) 2020 Alexandru Marc Serdeliuc

        https://github.com/serdeliuk/VirtualBiosMod

        VirtualBiosMod is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        VirtualBiosMod is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with VirtualBiosMod.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <efi.h>
#include <efilib.h>
#include "functions.c"

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int changes = 0;
    int vmajor = 1;
    int vminor = 0;
    int vpatch = 8;
    int params = 0;

    EFI_STATUS status;
    EFI_GUID guid_Setup = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } }; //Setup id  1
    EFI_GUID guid_SaSetup = { 0x72C5E28C, 0x7783, 0x43A1, { 0x87, 0x67, 0xFA, 0xD7, 0x3F, 0xCC, 0xAF, 0xA4 } }; //SaSetup id 2
    EFI_GUID guid_CpuSetup = { 0xB08F97FF, 0xE6E8, 0x4193, { 0xA9, 0x97, 0x5E, 0x9E, 0x9B, 0x0A, 0xDB, 0x32 } }; //CpuSetup id 3
    EFI_GUID guid_SystemConfig = { 0xA04A27F4, 0xDF00, 0x4D42, { 0xB5, 0x55, 0x39, 0x51, 0x13, 0x02, 0x11, 0x3D } }; //SystemConfig id 4
    EFI_GUID guid_PchSetup = { 0x4570B7F1, 0xADE8, 0x4943, { 0x8D, 0xC3, 0x40, 0x64, 0x72, 0x84, 0x23, 0x84 } }; //PchSetup id 5
	
    CHAR8 *data;
    UINTN data_size;
    UINT32 *attr = 0;
    EFI_INPUT_KEY efi_input_key;
    EFI_INPUT_KEY KeyReset = {0};
    EFI_INPUT_KEY efi_guid_key;

    EFI_STATUS efi_status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    BOOLEAN exit = FALSE;
    int num_input;
    int offset_lock = 0x17;
    int offset_audio = 0x55A;
    int offset_power_msr_lock = 0x2B;
    int offset_xtu = 0x1B8;
    int offset_overclock = 0x1B7;
    int offset_ratio = 0x1B9;
    int offset_ring = 0x1C4;
    int offset_maxring = 0x20B;
    int offset_avx = 0x1C2;
    int offset_cfg_lock = 0x3E;
    int offset_tdc_lock = 0x183;
    int offset_boot_perf_mode = 0xE;
    int offset_ecstates = 0x10;
    int offset_package_c_limit = 0x46;
    int offset_video = 0x13C;
    int offset_edram_mode = 0x110;
    int offset_dvmt_prealloc_memory = 0x107;
    int offset_dvmt_total_memory = 0x108;
    int offset_adaptive = 0x423;
	
    InitializeLib(image, systab);

    status = uefi_call_wrapper(systab->BootServices->HandleProtocol, 3, image, &LoadedImageProtocol, (void **) &loaded_image);
    if (EFI_ERROR(status)) {
            Print(L"[VirtualBiosMod] err: %r\n", status);
    } else {
	params = loaded_image->LoadOptionsSize;
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    if ( params == 0 ){
        Print(L"\n\n\n[VirtualBiosMod v%d.%d.%d]\n ",vmajor,vminor,vpatch);
    	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);
    	Print(L"Choose the variable store.\n");
    	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    	Print(L"Press 1 to choose Setup.\n");
    	Print(L"Press 2 to choose SaSetup.\n");
    	Print(L"Press 3 to choose CpuSetup.\n");
    	Print(L"Press 4 to choose SystemConfig.\n");
    	Print(L"Press 5 to choose PchSetup.\n");

    	WaitForSingleEvent(ST->ConIn->WaitForKey, 50000000); // 10000000 = one second

	while (!exit) {

    	    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_guid_key);
	    if (efi_status != EFI_SUCCESS) {
	        Print(L" Exiting\n\n\n");
	        return EFI_SUCCESS;
	    } else {
	        break;
	    }
        }
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

//  Print(L"123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_");
/////////--DRAW MAIN BLUE BOX--/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Print(L"             SERDELIUK - VirtualBiosMod v%d.%d.%d CMOS Setup Utility               ",vmajor,vminor,vpatch);
    draw_box_simple(80, 11, 0, 1);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 2); // h, v ;pos
    Print(L"Firmware version:       %d.%02d", ST->FirmwareRevision >> 16, ST->FirmwareRevision & 0xffff);
    Print(L" (%s)", ST->FirmwareVendor,L"%c"); // firmware vendoor

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 3); // h, v ;pos
    Print(L"UEFI version:           %d.%02d", ST->Hdr.Revision >> 16, ST->Hdr.Revision & 0xffff);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 15);

    switch (efi_input_key.UnicodeChar) {
	case '1':
    	    num_input = 1;		    
	case '2':
    	    num_input = 2;    		    
	case '3':
    	    num_input = 3;
	case '4':
    	    num_input = 4;
	case '5':
    	    num_input = 5;		    
    }
	if (num_input == 1) {
	    status = get_bios_variables( &guid_Setup, L"Setup", &data, &data_size, attr); //Setup id  1
	} else if ( num_input == 2 ) {
	    status = get_bios_variables( &guid_SaSetup, L"SaSetup", &data, &data_size, attr); //SaSetup id 2
	} else if ( num_input == 3 ) {
 	    status = get_bios_variables( &guid_CpuSetup, L"CpuSetup", &data, &data_size, attr); //CpuSetup id 3
	} else if ( num_input == 4 ) {
	    status = get_bios_variables( &guid_SystemConfig, L"SystemConfig", &data, &data_size, attr); //SystemConfig id 4
	} else if ( num_input == 5 ) {
   	    status = get_bios_variables( &guid_PchSetup, L"PchSetup", &data, &data_size, attr); //PchSetup id 5
	} else {
	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Wrong key entered/\n" , status);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	}
	
    if (status != EFI_SUCCESS) {
	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S.\n" , status);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
    }

redraw:
    WaitForSingleEvent(ST->ConIn->WaitForKey, 10); // 10000000 = one second
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

    	if (num_input == 1) {
    	    if ( data[offset_adaptive] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
		Print(L"Adaptive performance:   Disabled   ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
		Print(L"Adaptive performance:   Enabled    ");
    	    }
	} else if ( num_input == 2 ) {
    	    if ( data[offset_video] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
		Print(L"Video card:             IGFX -- Intel ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
		Print(L"Video card:             SG   -- Nvidia");
    	    }

	    if ( data[offset_dvmt_prealloc_memory] == 1) {
	        uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
	        	Print(L"DVMT Pre-Allocated:         32Mb      ");
        	    } else if ( data[offset_dvmt_prealloc_memory] == 2) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
        		Print(L"DVMT Pre-Allocated:         64Mb      ");
	    } else {
        		Print(L"DVMT Pre-Allocated:         Default   ");
    	    }

    	    if ( data[offset_dvmt_total_memory] == 1) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
        		Print(L"DVMT Total Memory:          128Mb     ");
            } else if ( data[offset_dvmt_total_memory] == 2) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
        		Print(L"DVMT Total Memory:          256Mb     ");
            } else {
        		Print(L"DVMT Total Memory:          MAX       ");
    	    }

    	    if ( data[offset_edram_mode] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
        		Print(L"eDram Mode:       SW Mode eDRAM Off");
        	    } else if ( data[offset_edram_mode] == 1) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
        		Print(L"eDram Mode:       SW Mode eDRAM  On");
        	    } else  {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
        		Print(L"eDram Mode:           eDRAM HW Mode");
    	    }
	} else if ( num_input == 3 ) {
    	    if ( data[offset_cfg_lock] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
        		Print(L"CFG Lock:               Disabled   ");
            } else {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
        		Print(L"CFG Lock:               Enabled    ");
    	    }

    	    if ( data[offset_boot_perf_mode] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
        		Print(L"Boot perf Mode:         Max Battery");
        	    } else if ( data[offset_boot_perf_mode] == 1) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
        		Print(L"Boot perf Mode:       Max Non-Turbo");
            } else  {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
        		Print(L"Boot perf Mode:          Turbo Perf");
    	    }

    	    if ( data[offset_package_c_limit] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C0/C1        ");
            } else if ( data[offset_package_c_limit] == 1) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C2           ");
            } else if ( data[offset_package_c_limit] == 2) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C3           ");
            } else if ( data[offset_package_c_limit] == 3) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C6           ");
            } else if ( data[offset_package_c_limit] == 4) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C7           ");
            } else if ( data[offset_package_c_limit] == 5) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C7s          ");
            } else if ( data[offset_package_c_limit] == 6) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C8           ");
            } else if ( data[offset_package_c_limit] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C9           ");
            } else {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
        		Print(L"Package C-state Limit:  C10          ");
    	    }
	
    	    if ( data[offset_xtu] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
		Print(L"Intel  XTU:             Disabled   ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
		Print(L"Intel  XTU:             Enabled    ");
    	    }

    	    if ( data[offset_ecstates] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
        		Print(L"Enhanced C-states:      Disabled   ");
            } else {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
        		Print(L"Enhanced C-states:      Enabled    ");
    	    }

    	    if ( data[offset_tdc_lock] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 10); // h, v ;pos
        		Print(L"TDC Lock:               Disabled   ");
        	    } else {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 10); // h, v ;pos
        		Print(L"TDC Lock:               Enabled    ");
    	    }

	    if ( data[offset_overclock] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
		Print(L"Overclock :             Disabled   ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
		Print(L"Overclock :             Enabled    ");
     	    }

	    if ( data[offset_ratio] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
		Print(L"Core RATIO:             Default    ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
		Print(L"Core RATIO:             53         ");
    	    }

	    if ( data[offset_ring] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 7); // h, v ;pos
		Print(L"Core Ring:              Default    ");
	   } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 7); // h, v ;pos
		Print(L"Core Ring:              53         ");
    	   }

	    if ( data[offset_maxring] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 8); // h, v ;pos
		Print(L"Max Ring:               Default    ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 8); // h, v ;pos
		Print(L"Max Ring:               53         ");
    	    }

	    if ( data[offset_avx] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
		Print(L"Max AVX:                Default    ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
		Print(L"Max AVX:                0x1F       ");
    	    }

    	    if ( data[offset_power_msr_lock] == 0) {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 10); // h, v ;pos
        		Print(L"Power Limit MSR Lock:   Disabled   ");
            } else {
        		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 10); // h, v ;pos
        		Print(L"Power Limit MSR Lock:   Enabled    ");
    	    }
	} else if ( num_input == 4 ) {
	    //SystemConfig id 4
	} else if ( num_input == 5 ) {
    	    if ( data[offset_lock] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
		Print(L"B.I.O.S. status:        Unlocked      ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
		Print(L"B.I.O.S. status:        Locked        ");
    	    }

    	    if ( data[offset_audio] == 0) {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
		Print(L"HDMI Audio:             Enabled       ");
	    } else {
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
		Print(L"HDMI Audio:             Disabled      ");
    	    }

	}

//      Print(L"123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_");

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 13);

	if ( num_input == 1 ) {
	    Print(L" Press A to enable/disable adaptive ratio\n");
	} else if ( num_input == 2 ) {
    	    Print(L" Press V to switch video card\n");
    	    Print(L" Press D to switch eDram Mode\n");
    	    Print(L" Press P to switc DVMT prealloc Memory\n");
    	    Print(L" Press T to switch DVMT Total Memory\n");
	} else if ( num_input == 3 ) {
	    Print(L" Press V to switch Package C-state Limit\n");
	    Print(L" Press I to enable/disable Intel XTU\n");
	    Print(L" Press O to enable/disable Overclocking\n");
	    Print(L" Press C to enable/disable CORE speed ratio\n");
	    Print(L" Press R to enable/disable CORE ring ratio\n");
	    Print(L" Press M to enable/disable MAX  ring ratio\n");
	    Print(L" Press X to enable/disable MAX  AVX ratio\n");
	    Print(L" Press P to enable/disable Power MSR Lock\n");
	    Print(L" Press F to enable/disable CFG Lock\n");
	    Print(L" Press T to enable/disable TDC Lock\n");
	    Print(L" Press Z to switch Boot perf Mode\n");   
	    Print(L" Press S to enable/disable Enhanced C-states\n");
	} else if ( num_input == 4 ) {

	} else if ( num_input == 5 ) {
	    Print(L" Press H to enable/disable HDMI audio\n");
	    Print(L" Press B to unlock the bios\n");
    }

    Print(L" Press ENTER to save new settings\n");
    if ( changes == 0 ) {
	Print(L" Press any other key or wait to boot without any mods\n");
    } else {
	Print(L" Press E to boot without any mods                    \n");
    }

    WaitForSingleEvent(ST->ConIn->WaitForKey, 50000000); // 10000000 = one second

    while (!exit) {

    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

    	if ( num_input == 1 ) {
	    switch (efi_input_key.UnicodeChar) {
		case 'a': ;
		    changes=1;
	    	    if ( data[offset_adaptive] == 0) {
			data[offset_adaptive] = 0x1;
	    	    } else {
			data[offset_adaptive] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
	    }
	} else if ( num_input == 2 ) {
	    switch (efi_input_key.UnicodeChar) {
 	        case 'p':		    
        	    changes=1;
            	    if ( data[offset_dvmt_prealloc_memory] == 1) {
                		data[offset_dvmt_prealloc_memory] = 0x2;
            	    } else {
                		data[offset_dvmt_prealloc_memory] = 0x1;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
        		case 't':
        		    changes=1;
            	    if ( data[offset_dvmt_total_memory] == 1) {
                		data[offset_dvmt_total_memory] = 0x2;
            	    } else if ( data[offset_dvmt_total_memory] == 2) {
                		data[offset_dvmt_total_memory] = 0x3;
	    	    } else {
                		data[offset_dvmt_total_memory] = 0x1;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
        	case 'd':
        	    changes=1;
            	    if ( data[offset_edram_mode] == 0) {
                	   	data[offset_edram_mode] = 0x1;
            	    } else if (data[offset_edram_mode] == 1) {
                		data[offset_edram_mode] = 0x2;
            	    } else {
                		data[offset_edram_mode] = 0x0;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
        	case 'v':
		    changes=1;
      	    	    if ( data[offset_video] == 0) {
		    	data[offset_video] = 0x4;
	    	    } else {
			data[offset_video] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
	    }
	} else if ( num_input == 3 ) {
	    switch (efi_input_key.UnicodeChar) {
		case 'p':
        	    changes=1;
            	    if ( data[offset_power_msr_lock] == 0) {
                		data[offset_power_msr_lock] = 0x1;
            	    } else {
                		data[offset_power_msr_lock] = 0x0;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
        	case 'f':
        	    changes=1;
            	    if ( data[offset_cfg_lock] == 0) {
                		data[offset_cfg_lock] = 0x1;
            	    } else {
                		data[offset_cfg_lock] = 0x0;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
       		case 's':
       		    changes=1;
            	    if ( data[offset_ecstates] == 0) {
                		data[offset_ecstates] = 0x1;
            	    } else {
                		data[offset_ecstates] = 0x0;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
        	case 't':
        	    changes=1;
            	    if ( data[offset_tdc_lock] == 0) {
                		data[offset_tdc_lock] = 0x1;
            	    } else {
                		data[offset_tdc_lock] = 0x0;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
       		case 'z':
        	    changes=1;
            	    if ( data[offset_boot_perf_mode] == 0) {
                		data[offset_boot_perf_mode] = 0x1;
            	    } else if (data[offset_boot_perf_mode] == 1) {
               	 	data[offset_boot_perf_mode] = 0x2;
	    	    } else {
			data[offset_boot_perf_mode] = 0x0;
            	    }
            	    efi_input_key = KeyReset;
            	    goto redraw;
       		case 'v':
		    changes=1;
	    	    if ( data[offset_package_c_limit] == 0) {
	 	        data[offset_package_c_limit] = 0x1;
	        	    } else if ( data[offset_package_c_limit] == 1) {
		        data[offset_package_c_limit] = 0x2;
	        	    } else if ( data[offset_package_c_limit] == 2) {
		    	data[offset_package_c_limit] = 0x3;
	        	    } else if ( data[offset_package_c_limit] == 3) {
		    	data[offset_package_c_limit] = 0x4;
	        	    } else if ( data[offset_package_c_limit] == 4) {
		    	data[offset_package_c_limit] = 0x5;
	        	    } else if ( data[offset_package_c_limit] == 5) {
		    	data[offset_package_c_limit] = 0x6;
	        	    } else if ( data[offset_package_c_limit] == 6) {
		    	data[offset_package_c_limit] = 0x7;
	        	    } else if ( data[offset_package_c_limit] == 0) {
		    	data[offset_package_c_limit] = 0x8;
	        	    }else {
		    	data[offset_package_c_limit] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;		    
       		case 'i':
		    changes=1;
	    	    if ( data[offset_xtu] == 0) {
			data[offset_xtu] = 0x1;
	    	    } else {
			data[offset_xtu] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
       		case 'o':
		    changes=1;
	    	    if ( data[offset_overclock] == 0) {
			data[offset_overclock] = 0x1;
	    	    } else {
			data[offset_overclock] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
       		case 'c':
		    changes=1;
	    	    if ( data[offset_ratio] == 0) {
			data[offset_ratio] = 0x53;
	    	    } else {
			data[offset_ratio] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
       		case 'r':
		    changes=1;
	    	    if ( data[offset_ring] == 0) {
			data[offset_ring] = 0x53;
	    	    } else {
			data[offset_ring] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
       		case 'm':
		    changes=1;
	    	    if ( data[offset_maxring] == 0) {
			data[offset_maxring] = 0x53;
	    	    } else {
			data[offset_maxring] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
         	case 'x':
		    changes=1;
	    	    if ( data[offset_avx] == 0) {
			data[offset_avx] = 0x1f;
	    	    } else {
			data[offset_avx] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
	    }
	} else if ( num_input == 4 ) {
	    switch (efi_input_key.UnicodeChar) {

	    }
	} else if ( num_input == 5 ) {
	    switch (efi_input_key.UnicodeChar) {
	        case 'b':
	  	    changes=1;
	    	    if ( data[offset_lock] == 0) {
			data[offset_lock] = 0x1;
	    	    } else {
			data[offset_lock] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
        	case 'h':
		    changes=1;
	    	    if ( data[offset_audio] == 0) {
			data[offset_audio] = 0x1;
	    	    } else {
			data[offset_audio] = 0x0;
	    	    }
	    	    efi_input_key = KeyReset;
	    	    goto redraw;
    }
		    
    }
    switch (efi_input_key.UnicodeChar) {
       case 'e':
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
        case CHAR_CARRIAGE_RETURN:
	    if ( changes == 0 ) {
    		Print(L" Nothing to save, booting......\n");
    		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	    }

    	    	if ( num_input == 1 ) {
	    	    status = set_bios_variables( L"Setup", &guid_Setup, data_size, data); //Setup id  1
		} else if ( num_input == 2 ) {
	    	    status = set_bios_variables( L"SaSetup", &guid_SaSetup, data_size, data); //SaSetup id 2
		} else if ( num_input == 3 ) {
 	    	    status = set_bios_variables( L"CpuSetup", &guid_CpuSetup, data_size, data); //CpuSetup id 3
		} else if ( num_input == 4 ) {
	    	    status = set_bios_variables( L"SystemConfig", &guid_SystemConfig, data_size, data); //SystemConfig id 4
		} else if ( num_input == 5 ) {
   	      	    status = set_bios_variables( L"PchSetup", &guid_PchSetup, data_size, data); //PchSetup id 5
	    } 

	    if (status != EFI_SUCCESS) {
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
		Print(L" ERROR saving data %r\n" , status);
		WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	    }
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
	    Print(L" Write data OK, rebooting...          \n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
    	    return EFI_SUCCESS;
        default: // continue boot
	    if ( changes == 1 ) {
		efi_input_key = KeyReset;
		goto redraw;
	    }
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 10000000); // 10000000 = one second
	    if ( params == 0){ 
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
        }
    }
    	    return EFI_SUCCESS;
}
