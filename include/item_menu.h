#ifndef ITEM_MENU_H
#define ITEM_MENU_H

void add_item(void);
void view_items(void);
void search_item(void);
void edit_item(void);
void delete_item(void);

//TODO: Maybe make a header to store these kinds of data ?
extern const int itm_cd_ln;
extern const int itm_nm_ln;

#endif