#include <gtest/gtest.h>
#include <Util/MVector.h>

using MVectorI = MVector<int, uint16_t, true>;

TEST(MVectorTests, RequireThat_MVector_Int_CanBeCreated)
{
    ASSERT_NO_THROW((MVectorI()));
}

TEST(MVectorTests, RequireThat_MVector_CretedEmpty)
{
    MVectorI instance;

    ASSERT_TRUE(instance.empty());
    ASSERT_EQ(0, instance.size());
}

TEST(MVectorTests, RequireThat_MVector_emplace_back_AddElements)
{
    MVectorI instance;
    ASSERT_EQ(0, instance.size());

    instance.emplace_back(1);
    ASSERT_EQ(1, instance.size());

    instance.emplace_back(3);
    ASSERT_EQ(2, instance.size());
}

