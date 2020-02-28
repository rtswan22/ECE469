#ifndef __USERPROG__
#define __USERPROG__

#define MSG     "hey what should we put here"
#define MSG_LEN 27

typedef struct mol_boxes {
  mbox_t mbox_S2;
  mbox_t mbox_S;
  mbox_t mbox_CO;
  mbox_t mbox_O2;
  mbox_t mbox_C2;
  mbox_t mbox_SO4;
}mol_boxes;

#define INJ_CO "inj_CO.dlx.obj"
#define INJ_S2 "inj_S2.dlx.obj"
#define RE_S2 "re_S2.dlx.obj"
#define RE_CO "re_CO.dlx.obj"
#define RE_SO4 "re_SO4.dlx.obj"


#endif
