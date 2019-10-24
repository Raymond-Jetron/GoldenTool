#pragma once
class CGoldenTable
{
public:
    CGoldenTable();
    ~CGoldenTable();

    static golden_int32 get_table_size(golden_int32 handle, const char *table_name);
	static golden_error get_points_id(golden_int32 handle, const char *table_name, golden_int32 *ids, golden_int32 *count);
	static golden_error get_points_id(golden_int32 handle, golden_int32 start_id, golden_int32 *ids, golden_int32 *count);
	static golden_error get_points_base_property(golden_int32 handle, golden_int32 count, GOLDEN_POINT *base);
};

