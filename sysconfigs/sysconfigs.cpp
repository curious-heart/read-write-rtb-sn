#include <QSet>
#include <QSettings>

#include "logger/logger.h"
#include "common_tools/common_tool_func.h"
#include "sysconfigs/sysconfigs.h"

#undef ENUM_NAME_DEF
#define ENUM_NAME_DEF(e) <<e

static const char* gs_str_value_range = "取值范围";

static const char* gs_sysconfigs_file_fpn = "configs/configs.ini";

static const char* gs_ini_grp_sys_cfgs = "sys_cfgs";
static const char* gs_ini_key_log_level = "log_level";

static const char* gs_ini_grp_op_cfgs = "op_cfgs";
static const char* gs_ini_key_sn_len = "sn_len";

/*the max allowed sn len is limited by mem space in device factory sector.*/
const int g_max_allowed_sn_len = 128; //8-bit ascii characters

sys_configs_struct_t g_sys_configs_block;

static const int gs_def_log_level = LOG_INFO;

static const int gs_def_sn_len = 20;

static RangeChecker<int> gs_cfg_file_log_level_ranger((int)LOG_DEBUG, (int)LOG_ERROR, "",
                     EDGE_INCLUDED, EDGE_INCLUDED);

static RangeChecker<int> gs_cfg_file_value_ge0_int_ranger(0, 0, "",
                           EDGE_INCLUDED, EDGE_INFINITE);

static RangeChecker<float> gs_cfg_file_value_ge0_float_ranger(0, 0, "",
                       EDGE_INCLUDED, EDGE_INFINITE);

static RangeChecker<int> gs_cfg_file_value_gt0_int_ranger(0, 0, "",
                       EDGE_EXCLUDED, EDGE_INFINITE);

static RangeChecker<int> gs_cfg_file_value_01_int_ranger(0, 1, "",
                       EDGE_INCLUDED, EDGE_INCLUDED);

static RangeChecker<float> gs_cfg_file_value_gt0_float_ranger(0, 0, "",
                       EDGE_EXCLUDED, EDGE_INFINITE);

static RangeChecker<int> gs_cfg_file_sn_ranger(0, g_max_allowed_sn_len, "",
                       EDGE_INCLUDED, EDGE_INCLUDED);

/*the __VA_ARGS__ should be empty or a type converter like (cust_type).*/
#define GET_INF_CFG_NUMBER_VAL(settings, key, type_func, var, def, factor, checker, checker_dtype, ...)\
{\
    cfg_ret = true; ret_str += (ret_str.isEmpty() ? "" : "\n");\
    (var) = __VA_ARGS__((factor) * ((settings).value((key), (def)).type_func()));\
    if((checker) && !((checker)->range_check((var))))\
    {\
        cfg_ret = false;\
        ret_str += QString("%1:%2\n%3 %4").arg((key)).arg((var)).arg(gs_str_value_range, (checker)->range_str(checker_dtype));\
    }\
    ret = ret && cfg_ret;\
}

/*check the validation of config parameters.*/
#define CHECK_LIMIT_RANGE(l_name, min_l, max_l, checker, unit_str) \
    cfg_ret = true; \
    if(((checker) && (!((checker)->range_check(min_l)) || !((checker)->range_check(max_l)))) \
        || ((min_l) > (max_l)))\
    {\
        cfg_ret = false;\
        ret_str += QString((l_name)) + \
                   " [" + QString::number((min_l)) + ", " + QString::number((max_l)) + "] " +\
                   (unit_str) + "\n";\
    }\
    ret = ret && cfg_ret;

/**/
#define CHECK_ENUM(title_str, e_v, e_set, str_func) \
    cfg_ret = true; ret_str += (ret_str.isEmpty() ? "" : "\n");\
    if(!e_set.contains(e_v))\
    {\
        cfg_ret = false;\
        ret_str += QString(gs_str_cfg_item) + (title_str) + "\n" + gs_str_should_be_one_val_of + "\n{";\
        auto it = e_set.constBegin();\
        while(it != e_set.constEnd()) {ret_str += str_func(*it) + ", "; ++it;}\
        ret_str.chop(2);\
        ret_str += "}\n";\
        ret_str += QString(gs_str_actual_val) + str_func(e_v) + "\n";\
    }\
    ret = ret && cfg_ret;

bool fill_sys_configs(QString * ret_str_ptr)
{
    bool ret = true, cfg_ret = true;
    QString ret_str;
    QSettings settings(gs_sysconfigs_file_fpn, QSettings::IniFormat);

    /*--------------------*/
    settings.beginGroup(gs_ini_grp_sys_cfgs);
    GET_INF_CFG_NUMBER_VAL(settings, gs_ini_key_log_level, toInt,
                           g_sys_configs_block.log_level, gs_def_log_level,
                           1, &gs_cfg_file_log_level_ranger, INT_DATA);
    settings.endGroup();

    /*--------------------*/
    settings.beginGroup(gs_ini_grp_op_cfgs);

    GET_INF_CFG_NUMBER_VAL(settings, gs_ini_key_sn_len, toInt,
                           g_sys_configs_block.sn_len, gs_def_sn_len,
                           1, &gs_cfg_file_sn_ranger, INT_DATA);
    settings.endGroup();

    /*--------------------*/

    if(ret_str_ptr) *ret_str_ptr = ret_str;
    return ret;
}
