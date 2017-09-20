/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************/


#include "rt_config.h"




/*
	========================================================================

	Routine Description:

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
void RTInitializeCmdQ(struct rtmp_command_queue *cmdq)
{
	cmdq->head = NULL;
	cmdq->tail = NULL;
	cmdq->size = 0;
	cmdq->CmdQState = RTMP_TASK_STAT_INITED;
}


/*
	========================================================================

	Routine Description:

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
void RTThreadDequeueCmd(struct rtmp_command_queue *cmdq,
	struct rtmp_queue_elem **pcmdqelmt)
{
	*pcmdqelmt = cmdq->head;

	if (*pcmdqelmt != NULL)
	{
		cmdq->head = cmdq->head->next;
		cmdq->size--;
		if (cmdq->size == 0)
			cmdq->tail = NULL;
	}
}


/*
	========================================================================

	Routine Description:

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
int RTEnqueueInternalCmd(
	IN struct rtmp_adapter *pAd,
	IN NDIS_OID			Oid,
	IN void *		pInformationBuffer,
	IN u32			InformationBufferLength)
{
	int	status;
	struct rtmp_queue_elem *cmdqelmt = NULL;


	cmdqelmt = kmalloc(sizeof(*cmdqelmt), GFP_ATOMIC);
	if (!cmdqelmt)
		return (NDIS_STATUS_RESOURCES);
	memset(cmdqelmt, 0, sizeof(*cmdqelmt));

	if(InformationBufferLength > 0) {
		cmdqelmt->buffer = kmalloc(InformationBufferLength, GFP_ATOMIC);
		if (!cmdqelmt->buffer) 	{
			kfree(cmdqelmt);
			return (NDIS_STATUS_RESOURCES);
		} else {
			memmove(cmdqelmt->buffer, pInformationBuffer, InformationBufferLength);
			cmdqelmt->bufferlength = InformationBufferLength;
		}
	} else {
		cmdqelmt->buffer = NULL;
		cmdqelmt->bufferlength = 0;
	}

	cmdqelmt->command = Oid;

	if (cmdqelmt != NULL) {
		spin_lock_bh(&pAd->CmdQLock);
		if (pAd->CmdQ.CmdQState & RTMP_TASK_CAN_DO_INSERT) {
			EnqueueCmd((&pAd->CmdQ), cmdqelmt);
			status = NDIS_STATUS_SUCCESS;
		} else {
			status = NDIS_STATUS_FAILURE;
		}
		spin_unlock_bh(&pAd->CmdQLock);

		if (status == NDIS_STATUS_FAILURE) {
			if (cmdqelmt->buffer)
				kfree(cmdqelmt->buffer);
			kfree(cmdqelmt);
		} else {
			RTCMDUp(&pAd->cmdQTask);
		}
	}
	return(NDIS_STATUS_SUCCESS);
}
