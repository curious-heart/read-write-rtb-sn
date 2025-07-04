#ifndef SYSCONFIGS_H
#define SYSCONFIGS_H

#include <QString>

#define ENUM_NAME_DEF(e) e,

typedef struct
{
    int log_level;

    int sn_len;
}sys_configs_struct_t;

extern const int g_max_allowed_sn_len;
extern sys_configs_struct_t g_sys_configs_block;

bool fill_sys_configs(QString *);

#endif // SYSCONFIGS_H
