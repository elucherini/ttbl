#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


// MASKS: GENERIC
#define TTBL_INVALIDATE_ENTRY_MASK	0xfffffffffffffffe
#define TTBL_VALIDATE_ENTRY_MASK	0x0000000000000001
#define TTBL_IS_VALID_ENTRY_MASK	TTBL_VALIDATE_ENTRY_MASK //(addr & MASK != 0? VALID : NOT)

// LEVEL 0, 1, 2 (BLOCK, TABLE)
#define TTBL_BLOCK_ENTRY_MASK		0xfffffffffffffffd
#define TTBL_TABLE_ENTRY_MASK		0x0000000000000002
#define TTBL_IS_TABLE_ENTRY_MASK	TTBL_TABLE_ENTRY_MASK //(addr & MASK != 0? TABLE : BLOCK)


// LEVEL 3 (PAGE)
#define TTBL_PAGE_LOWER_ATTR_MASK	0x0000000000000ffc
#define TTBL_PAGE_LOWER_ATTR_SHIFT	2
#define TTBL_PAGE_OA_MASK			0x0000fffffffff000
#define TTBL_PAGE_OA_SHIFT			12
#define TTBL_PAGE_UPPER_ATTR_MASK	0xfff8000000000000
#define TTBL_PAGE_UPPER_ATTR_SHIFT	51


// BLOCK
#define TTBL_BLOCK_LOWER_ATTR_MASK	TTBL_PAGE_LOWER_ATTR_MASK // (addr & MASK = isolates [11:2])
#define TTBL_BLOCK_LOWER_ATTR_SHIFT TTBL_PAGE_LOWER_ATTR_SHIFT
#define TTBL_BLOCK_UPPER_ATTR_MASK	0xfff0000000000000 // (addr & MASK = isolates [63:52])
#define TTBL_BLOCK_UPPER_ATTR_SHIFT	52
#define TTBL_L1BLOCK_OA_MASK		0x0000ffffc0000000 // (addr & MASK = isolates [47:30])
#define TTBL_L1BLOCK_OA_SHIFT		30
#define TTBL_L2BLOCK_OA_MASK		0x0000ffffffe00000 // (addr & MASK = isolates [47:21])
#define TTBL_L2BLOCK_OA_SHIFT		21

// BLOCK/PAGE ATTRIBUTES: COMMON
#define TTBL_UPPER_PBHA_MASK		0x7800000000000000
#define TTBL_UPPER_PBHA_SHIFT		59
#define TTBL_LOWER_AF_MASK			0x0000000000000400
#define TTBL_LOWER_AF_SHIFT			10
#define TTBL_LOWER_SH_MASK			0x0000000000000300
#define TTBL_LOWER_SH_SHIFT			8
#define TTBL_LOWER_NG_MASK			0x0000000000000800	// NG set to 1 in stage 2
#define TTBL_LOWER_NG_SHIFT			11


// BLOCK/PAGE ATTRIBUTES: STAGE 1
#define TTBL_ST1_UPPER_UXN_MASK			0x0040000000000000
#define TTBL_ST1_UPPER_UXN_SHIFT		54
#define TTBL_ST1_UPPER_PXN_MASK			0x0020000000000000
#define TTBL_ST1_UPPER_PXN_SHIFT		53
#define TTBL_ST1_UPPER_CONT_MASK		0x0010000000000000
#define TTBL_ST1_UPPER_CONT_SHIFT		52
#define TTBL_ST1_UPPER_DBM_MASK			0x0008000000000000
#define TTBL_ST1_UPPER_DBM_SHIFT		51
#define TTBL_ST1_LOWER_AP_MASK			0x00000000000000c0
#define TTBL_ST1_LOWER_AP_SHIFT			6
#define TTBL_ST1_LOWER_NS_MASK			0x0000000000000020
#define TTBL_ST1_LOWER_NS_SHIFT			5
#define TTBL_ST1_LOWER_ATTRIDX_MASK		0x000000000000001c
#define TTBL_ST1_LOWER_ATTRIDX_SHIFT	2

// BLOCK/PAGE ATTRIBUTES: STAGE 2
#define TTBL_ST2_UPPER_XN_MASK 			0x0060000000000000
#define TTBL_ST2_UPPER_XN_SHIFT			54
#define TTBL_ST2_UPPER_CONT_MASK		0x0010000000000000
#define TTBL_ST2_UPPER_CONT_SHIFT		52
#define TTBL_ST2_UPPER_DBM_MASK			0x0008000000000000
#define TTBL_ST2_UPPER_DBM_SHIFT		51
#define TTBL_ST2_LOWER_S2AP_MASK		TTBL_ST1_LOWER_AP_MASK
#define TTBL_ST2_LOWER_S2AP_SHIFT		TTBL_ST1_LOWER_AP_SHIFT
#define TTBL_ST2_LOWER_ATTRIDX_MASK		0x000000000000003c
#define TTBL_ST2_LOWER_ATTRIDX_SHIFT	2

// TABLE
#define TTBL_TABLE_NEXTTBL_MASK		0x0000fffffffff000 // (addr & MASK = isolates [47:12])
#define TTBL_TABLE_NEXTTBL_SHIFT	12

// TABLE STAGE 1 (RES IN STAGE 2)
#define TTBL_PXNTABLE_MASK			0x0800000000000000
#define TTBL_PXNTABLE_SHIFT			59
#define TTBL_XNTABLE_MASK 			0x1000000000000000
#define TTBL_XNTABLE_SHIFT 			60
#define TTBL_APTABLE_MASK			0x6000000000000000
#define TTBL_APTABLE_SHIFT			61
#define TTBL_NSTABLE_MASK			0x8000000000000000
#define TTBL_NSTABLE_SHIFT			63




uint64_t ttbl_entry_set_invalid (uint64_t *addr)
{
	*addr &= TTBL_INVALIDATE_ENTRY_MASK;
}

bool ttbl_entry_is_valid (uint64_t addr)
{
	return (addr & TTBL_IS_VALID_ENTRY_MASK);
}

bool ttbl_entry_is_table (uint64_t addr)
{
	// TODO: check that entry belongs to level 0, 1, 2
	return addr & TTBL_IS_TABLE_ENTRY_MASK;
}

bool ttbl_entry_is_block (uint64_t addr)
{
	// TODO: check that entry belongs to level 0, 1, 2
	return !ttbl_entry_is_table(addr);
}

int main()
{
	/* TEST PROGRAM */
	uint64_t a = 0x0000f44400004445LL;
	uint64_t b = 0x0000f44400004446LL;

	// TEST VALIDATION/INVALIDATION
	/*
	if (ttbl_entry_is_valid(a))
		printf("a is valid: 0x%016llx\n", a);
	// invalidate i
	printf("Invalidate a...\n");
	ttbl_entry_set_invalid(&a);
	//a &= TTBL_INVALIDATE_ENTRY_MASK;
	if (!ttbl_entry_is_valid(a))
		printf("a becomes invalid: 0x%016llx\n", a);
	// validate i
	printf("Validate a again...\n");
	a |= TTBL_VALIDATE_ENTRY_MASK;
	if (ttbl_entry_is_valid(a))
		printf("a becomes valid: 0x%016llx\n", a);
	*/

	// LEVEL 0, 1, 2

	// TEST BLOCK/TABLE
	
	if (ttbl_entry_is_table(b))
		printf("b is a table: 0x%016llx\n", b);
	// convert b into block
	printf("Convert b into a block...\n");
	b &= TTBL_BLOCK_ENTRY_MASK;
	if (ttbl_entry_is_block(b))
		printf("b becomes a block: 0x%016llx\n", b);
	// convert b into table
	printf("Convert b back into a table...\n");
	b |= TTBL_TABLE_ENTRY_MASK;
	if (!ttbl_entry_is_block(b))
		printf("b becomes a table: 0x%016llx\n", b);
	
}
