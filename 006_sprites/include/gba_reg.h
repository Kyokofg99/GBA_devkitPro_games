//---------------------------------------------------------------------------------
//This class defines the base register addresses for I/O VRAM etc
//---------------------------------------------------------------------------------

#ifndef __GBA_REG_H__
#define __GBA_REG_H__

#include "gba_types.h"
/*
* Define the Base Address for External (on-board) Work RAM (WRAM)
* This is 256K in Size
*/
#define	EWRAM		     0x02000000
#define	EWRAM_END	     0x02040000 // From This address up to 0x02FFFFFF is not utilised
/*
* Define the Base Address for Internal (on-chip) Work RAM (WRAM)
* This is 32K in Size
*/
#define IWRAM            0x03000000
/*
* Define the Base Address for I/O or Hardware Registers
*
*/
#define REG_BASE         0x04000000

#define MEM_PALETTE      ((u16*)(0x05000200))

/*
* Define the Base Address for Video RAM (VRAM)
* This is 96K in Size
*/
#define VRAM             0x06000000
#define MEM_TILE         ((TileBlock*)0x6000000 )

#define MEM_OAM          ((volatile object_attributes_t *)0x07000000)
#define OAM              ((volatile object_attributes_t *)0x07000000)

/*
* Define the Base Address for Cart Save RAM (VRAM)
* This is 64K (Max) in Size
*/
#define	SRAM		      0x0E000000



#endif //__GBA_REG_H__