// Last Update:2018-12-26 17:20:10
/**
 * @file adts.c
 * @brief 
 * @author liyq
 * @version 0.1.00
 * @date 2018-12-16
 */

#include <string.h>
#include <arpa/inet.h>
#include "dbg_internal.h"
#include "adts.h"

#define ADTS_SYNCWORKD 0xFFF

int AacDecodeAdts( char *_pFrame, int _nLen, OUT Adts *_pAdts, int *_pSize )
{
    char *pStart = _pFrame, *pEnd = pStart + _nLen;
    int nAacFrameLen = 0, nHeaderLen = 0;
    Adts *pAdtsAddr = _pAdts;
    unsigned int *pSyncWord = NULL;

    if ( !_pFrame || !_pAdts || !_pSize || _nLen <= 0 ) {
        return ADTS_PARAM_ERROR;
    }

    while( pStart < pEnd ) {
        pSyncWord = (unsigned int *)pStart;
        if ( ( (htonl(*pSyncWord) >> 20) & 0xFFF ) == ADTS_SYNCWORKD ) {
            nAacFrameLen = 0;
            nAacFrameLen |= (pStart[3] & 0x3) << 11;
            nAacFrameLen |= pStart[4] << 3;
            nAacFrameLen |= (pStart[5] & 0xe0) >> 5;
            nHeaderLen = (pStart[1] & 0x1) == 1 ? 7 : 9; // check whether there is crc
            _pAdts->size = nAacFrameLen - nHeaderLen;// nAacFrameLen = headerlen + aacDataLen 
            _pAdts->addr = pStart + nHeaderLen;
            pStart += nAacFrameLen;
            _pAdts++;
            if ( _pAdts - pAdtsAddr >= *_pSize ) {
                *_pSize = _pAdts - pAdtsAddr;
                return ADTS_OVERFLOW;
            }
        }
    }

    *_pSize = _pAdts - pAdtsAddr;

    return ADTS_DECODE_OK;
}
