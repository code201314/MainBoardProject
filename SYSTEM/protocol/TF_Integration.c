#include "TinyFrame.h"
#include "usart.h"

/**
 * This is an example of integrating TinyFrame into the application.
 * 
 * TF_WriteImpl() is required, the mutex functions are weak and can
 * be removed if not used. They are called from all TF_Send/Respond functions.
 * 
 * Also remember to periodically call TF_Tick() if you wish to use the 
 * listener timeout feature.
 */
bool do_corrupt = false;
void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
	
    // send to UART
	// usart_write(USART1, buff, len);
    printf("--------------------\n");
    printf("\033[32mTF_WriteImpl - sending frame:\033[0m\n");
    
    uint8_t *xbuff = (uint8_t *)buff;    
    if (do_corrupt) {
        printf("(corrupting to test checksum checking...)\n");
        xbuff[8]++;
    }
    
    dumpFrame(xbuff, len);

    // Send it back as if we received it
    TF_Accept(tf, xbuff, len);
}

// --------- Mutex callbacks ----------
// Needed only if TF_USE_MUTEX is 1 in the config file.
// DELETE if mutex is not used

/** Claim the TX interface before composing and sending a frame */
bool TF_ClaimTx(TinyFrame *tf)
{
    // take mutex
    return true; // we succeeded
}

/** Free the TX interface after composing and sending a frame */
void TF_ReleaseTx(TinyFrame *tf)
{
    // release mutex
}

// --------- Custom checksums ---------
// This should be defined here only if a custom checksum type is used.
// DELETE those if you use one of the built-in checksum types

/** Initialize a checksum */
TF_CKSUM TF_CksumStart(void)
{
    return 0;
}

/** Update a checksum with a byte */
TF_CKSUM TF_CksumAdd(TF_CKSUM cksum, uint8_t byte)
{
    return cksum ^ byte;
}

/** Finalize the checksum calculation */
TF_CKSUM TF_CksumEnd(TF_CKSUM cksum)
{
    return cksum;
}
