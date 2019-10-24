#include "GoldenTable.h"


CGoldenTable::CGoldenTable()
{}


CGoldenTable::~CGoldenTable()
{}

golden_int32 CGoldenTable::get_table_size(golden_int32 handle, const char* table_name)
{
    golden_int32 table_size = 0;
    golden_error ecode = GoE_FALSE;
    if (table_name)
    {
        //根据表名称获取表中包含的标签点数量
        ecode = gob_get_table_size_by_name(handle, table_name, &table_size);
        goldencommon::check_ecode(ecode, _T("gob_get_table_size_by_name"));
    }
    else
    {
        //获取全库标签点数量
        golden_int32 table_count = 100, cur_table_size = 0;
        vector<golden_int32> table_id(table_count);
        ecode = gob_get_tables(handle, table_id.data(), &table_count);
        for (int i = 0; i < table_count; ++i)
        {
            gob_get_table_size_by_id(handle, table_id[i], &cur_table_size);
            table_size += cur_table_size;
        }
    }
    return table_size;
}

golden_error CGoldenTable::get_points_id(golden_int32 handle, const char *table_name, golden_int32 *ids, golden_int32 *count)
{
    golden_error ecode = GoE_FALSE;
    ecode = gob_search(handle, "*", table_name, NULL, NULL, NULL, NULL, GOLDEN_SORT_BY_ID, ids, count);
    if (ecode != GoE_OK)
    {
        cout << "search " << table_name << " failed";
        goldencommon::check_ecode(ecode, _T("gob_search"));
    }
    if (count == 0)
    {
        cout << "search nothing." << endl;
    }
    return ecode;
}

golden_error CGoldenTable::get_points_id(golden_int32 handle, golden_int32 start_id, golden_int32 *ids, golden_int32 *count)
{
	golden_error ecode = GoE_FALSE;
	for (int i = 0; i < *count; ++i)
	{
		ids[i] = start_id++;
	}
	return ecode;
}


golden_error CGoldenTable::get_points_base_property(golden_int32 handle, golden_int32 count, GOLDEN_POINT *base)
{
	golden_error ecode = GoE_FALSE;
	vector<golden_error> errors(count);
	ecode = gob_get_points_property(handle, count, base, nullptr, nullptr, errors.data());
	goldencommon::check_ecode(ecode, _T("gob_get_points_property"));
	return ecode;
}
