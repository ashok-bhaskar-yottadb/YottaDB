/****************************************************************
 *								*
 * Copyright (c) 2001-2015 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 * Copyright (c) 2017 YottaDB LLC. and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include "gtm_stdlib.h"		/* for EXIT() */
#include "gtm_string.h"

#include "gtmimagename.h"
#include "send_msg.h"
#include "getzposition.h"
#include "op.h"

GBLREF	int		mumps_status;

LITREF	gtmImageName	gtmImageNames[];

error_def(ERR_PROCTERM);

void op_halt(void)
{
	GTMTRIG_ONLY(mval	zposition;)
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
#	ifdef GTM_TRIGGER
	/* If HALT is done from a non-runtime trigger, send a warning message to oplog to record the fact
	 * of this uncommon process termination method.
	 */
	if (!IS_GTM_IMAGE)
        {
		zposition.mvtype = 0;   /* It's not an mval yet till getzposition fills it in */
		getzposition(&zposition);
		assert(MV_IS_STRING(&zposition) && (0 < zposition.str.len));
		send_msg_csa(CSA_ARG(NULL) VARLSTCNT(9) ERR_PROCTERM, 7, GTMIMAGENAMETXT(image_type), RTS_ERROR_TEXT("HALT"),
			 0, zposition.str.len, zposition.str.addr);
	}
#	endif
	if (0 < TREF(gtmci_nested_level))
	{	/* Call-in is active - Need to return to caller - not halt */
		mumps_status = 0;
		op_zg1(0);			/* Unwind everything back to beginning of this call-in level */
		assertpro(FALSE);		/* Should not return */
		return;				/* Previous call does not return so this is for the compiler */
	}
	EXIT(EXIT_SUCCESS);
}
