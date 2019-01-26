#include "player_channel.h"
#include "player_protocol.h"
#include "player_role.h"
#include "player_message.h"
#include "msg.pb.h"

PlayerMsg * PlayerProtocol::GetMessagefromRaw()
{
    PlayerMsg *pxRet = NULL;
    if (8 <= m_lasttime_data.iLength - iCurPos)
    {
        int ilength = m_lasttime_data.pucData[0 + iCurPos]|
                      m_lasttime_data.pucData[1 + iCurPos]|
                      m_lasttime_data.pucData[2 + iCurPos]|
                      m_lasttime_data.pucData[3 + iCurPos];

        int iId =     m_lasttime_data.pucData[4 + iCurPos]|
                      m_lasttime_data.pucData[5 + iCurPos]|
                      m_lasttime_data.pucData[6 + iCurPos]|
                      m_lasttime_data.pucData[7 + iCurPos];
                      
        if (ilength <= m_lasttime_data.iLength - iCurPos - 8)
        {
            std::string szRaw = std::string((char *)(m_lasttime_data.pucData + 8 + iCurPos), ilength);
            pxRet = new PlayerMsg(iId);
            if (3 == iId)
            {
                pb::Position *pxPosMsg = new pb::Position();
                pxRet->pxProtobufMsg = pxPosMsg;
            }
            if (2 == iId)
            {
                pb::Talk *pxTalkMsg = new pb::Talk();
                pxRet->pxProtobufMsg = pxTalkMsg;
            }
            pxRet->pxProtobufMsg->ParseFromArray((char *)(m_lasttime_data.pucData + 8 + iCurPos),ilength);
            iCurPos += ilength + 8;
        }
    }

    return pxRet;
}

PlayerProtocol::PlayerProtocol(PlayerRole *_bindPlayer):pxBindRole(_bindPlayer)
{
}
bool PlayerProtocol::raw2request(const RawData *pstData, std::list<Request *> &_ReqList)
{
    bool bRet = false;

    if (true == m_lasttime_data.AppendData(pstData))
    {
        PlayerMsg *pxMsg = NULL;
        while (NULL != (pxMsg = GetMessagefromRaw()))
        {
            Request *pstReq = new Request();
            pstReq->pxMsg = pxMsg;
            pstReq->pxProcessor = pxBindRole;
            _ReqList.push_back(pstReq);
            bRet = true;
        }
        if (iCurPos < m_lasttime_data.iLength)
        {
            unsigned char *pucTmpData = (unsigned char *)calloc(1UL, m_lasttime_data.iLength - iCurPos);
            if (NULL != pucTmpData)
            {
                memcpy(pucTmpData, m_lasttime_data.pucData, m_lasttime_data.iLength - iCurPos);
                free(m_lasttime_data.pucData);
                m_lasttime_data.pucData = pucTmpData;
                m_lasttime_data.iLength = m_lasttime_data.iLength - iCurPos;
            }
        }
        else
        {
            free(m_lasttime_data.pucData);
            m_lasttime_data.pucData = NULL;
            m_lasttime_data.iLength = 0;
        }
        iCurPos = 0;
    }
    
    return bRet;
}

bool PlayerProtocol::response2raw(const Response * pstResp, RawData * pstData)
{
    bool bRet = false;
    PlayerMsg *pxMsg = dynamic_cast<PlayerMsg *>(pstResp->pxMsg);
    int iProtoLen = 0;
    
    if (NULL != pxMsg)
    {
        std::string szOut;
        if (true == pxMsg->pxProtobufMsg->SerializeToString(&szOut))
        {
            iProtoLen = szOut.size();
            unsigned char *pucData = (unsigned char *)calloc(1UL, iProtoLen + 8);
            pucData[0] = iProtoLen & 0xff;
            pucData[1] = (iProtoLen>>8) & 0xff;
            pucData[2] = (iProtoLen>>16) & 0xff;
            pucData[3] = (iProtoLen>>24) & 0xff;
            pucData[4] = (pxMsg->Id) & 0xff;
            pucData[5] = (pxMsg->Id>>8) & 0xff;
            pucData[6] = (pxMsg->Id>>16) & 0xff;
            pucData[7] = (pxMsg->Id>>24) & 0xff;
            szOut.copy((char *)(pucData + 8), iProtoLen, 0);
            pstData->iLength = iProtoLen + 8;
            pstData->pucData = pucData;
            bRet = true;
        }
    }
    
    
    return bRet;
}

