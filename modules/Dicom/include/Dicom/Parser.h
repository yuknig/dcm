#ifndef _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
#define _PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_

#include <Dicom/TagValues.h>
#include <Dicom/TagStruct/ParseConfig.h>

#include <memory>
#include <assert.h>
#include <Util/Stream.h>

namespace dcm
{

class Parser
{
public://functions
    using GroupPtr = std::shared_ptr<Group>;

    Parser(StreamRead& a_stream, const Tag& a_max_tag = 0xffffffff);

    GroupPtr root() const;

protected://functions
    // move to testable ParserHelper
    static bool Parse(StreamRead& a_stream, GroupPtr& a_root, const Tag& a_max_tag);

private://data
    GroupPtr     m_root;
    ParserConfig m_config;
};

}//namespace dcm

#endif //_PARSER_C6B68E8E_AFAF_499F_95AE_249B47CE139F_
