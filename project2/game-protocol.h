/*
 * game-protocol.h
 * Version 20160520
 * Written by Harry Wong (harryw1)
 */

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define GP_SIZE        192
#define GP_HEADER_SIZE 7
#define GP_DISP_MSG    "DISPMSG"
#define GP_GUESS_REQ   "REQUEST"
#define GP_GUESS_FBK   "FEEDBAC"
#define GP_GUESS_INV   "INVALID"
#define GP_GAME_SUCC   "SUCCESS"
#define GP_GAME_FAIL   "FAILURE"

#define GP_PAYLOAD_SIZE GP_SIZE - GP_HEADER_SIZE
