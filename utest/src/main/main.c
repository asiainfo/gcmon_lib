
#include "utest.h"

static void urun()
{
    GPublic void utest_rbtree_run();
    GPublic void utest_list_run();
    UItem_t aItems[] =
    {
        { 0, TRUE, NULL, NULL, "�˳���Ԫ����" },
        { 1, TRUE, utest_rbtree_run, NULL, "�����������Ե�Ԫ" },
        { 2, TRUE, utest_list_run, NULL, "�����������Ե�Ԫ" },
    };

    utest_run(aItems, ARRAY_SIZE(aItems));
}

int main(int argc, char **argv, char **env)
{
    urun();
    return 0;
}