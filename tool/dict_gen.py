import xml.etree.ElementTree as ET
import urllib2

class tag_def:
    def __init__(self, columns):
        if len(columns) < 5:
            raise Exception('missing columns')
        self.group_num, self.elm_num, __, __ = self.parse_tag_num(columns[0])
        self.vrs = self.parse_vr(columns[3])
        self.keyword = columns[2]
        self.name = columns[1]
        if (not self.keyword or not self.name):
            raise Exception('invalid keyword or name')

    #parse tag number given in format: '(XXXX,YYYY)'
    def parse_tag_num(self, str):
        if len(str) != 11 or str[0] != '(' or str[-1] != ')' or str[5] != ',':
            raise Exception('wrong tag number format')

        group_num_str   = str[1:5].upper()
        element_num_str = str[6:10].upper()
        group_num_mask = 0
        element_num_mask = 0
        for digit in range(4):
            if group_num_str[digit] == 'X':
                group_num_str = group_num_str[:digit] + '0' + group_num_str[digit+1:];
            else:
                group_num_mask |= 1 << (3 - digit)

            if element_num_str[digit] == 'X':
                element_num_str = element_num_str[:digit] + '0' + element_num_str[digit+1:]
            else:
                element_num_mask |= 1 << (3 - digit)

        return (int(group_num_str, 16), int(element_num_str, 16), group_num_mask, element_num_mask)

    def parse_vr(self, str):
        vr_list = ['AE', 'AS', 'AT', 'CS', 'DA', 'DS', 'DT', 'FL', 'FD', 'IS', 'LO', 'LT', 'OB', 'OD', 'OF', 'OL', 'OW', 'PN', 'SH', 'SL', 'SQ', 'SS', 'ST', 'TM', 'UC', 'UI', 'UL', 'UN', 'UR', 'US', 'UT']
        result = []
        tokens = str.upper().split(' ')
        for token in tokens:
            if (token == 'OR'):
                continue
            if token not in vr_list:
                raise Exception('unknown VR:' + token)
            result.append(token)
        return result

def download_xml_dict(num_attempts):
    for _ in range(num_attempts):
        try:
            print('Acquiring dictionary...')
            # loading from nema website
            response = urllib2.urlopen('http://dicom.nema.org/medical/dicom/current/source/docbook/part06/part06.xml')
            xml_content = response.read()

            # loading from local file
            #with open('F:/develop/part06.xml') as f:
            #    xml_content = f.read()

            print('Parsing file...')
            result = ET.fromstring(xml_content)
            return result
        except Exception as e:
            print(e)
            continue
    print('Failed to download dictionary')

if __name__ == '__main__':
    root = download_xml_dict(3)

    print('Processing...')
    nsps = {'ns' : 'http://docbook.org/ns/docbook'} #namespaces
    items = root.findall('ns:chapter/ns:table/ns:tbody/ns:tr', nsps)

    tags = []
    vr_set = set()
    for item in items:
        if len(item._children) < 5:
            continue
        cur_row = item.findall('ns:td/ns:para', nsps)
        if len(cur_row) < 5:
            continue

        text_in_cols = [''] * 5

        for col_num in range(0, min(len(cur_row), 5)):
            col = cur_row[col_num]
            nested_col = col.find('ns:emphasis', nsps)
            if nested_col is not None:
                col = nested_col
            if col.text is not None:
                text_in_cols[col_num] = col.text.encode('ascii', 'ignore').decode('ascii')

        try:
            item = tag_def(text_in_cols)
            tags.append(item)
            vrs = '_'.join(item.vrs)
            if (vrs not in vr_set):
                vr_set.add(vrs)
        except Exception as e:
            print('failed to parse tag ', text_in_cols, ':', e)
            continue

    tags.sort(key = lambda x: (x.group_num, x.elm_num))

    print('generated file:')
    print('\n//file generated automatically by dict_gen.py script')
    print('#pragma once')
    print('#include <Dicom/TagStruct/Vr.h>')
    print('')
    print('namespace dcm')
    print('{')
    print('')
    print('namespace dict')
    print('{')

    print('')
    print('///////////////////')
    print('// tags definitions')
    print('///////////////////')
    max_keyword_len = max(len(x.keyword) for x in tags)
    max_name_len = max(len(x.name) for x in tags)
    for tag in tags:
        tag_str = 'static const unsigned int {0:<{1}} = 0x{2:04x}{3:04x};'.format(tag.keyword, max_keyword_len, tag.group_num, tag.elm_num)
        print(tag_str)

    print('')
    print('')
    print('///////////////////')
    print('// used VRs')
    print('///////////////////')
    print('namespace tag_vr')
    print('{\n')
    vr_list = list(vr_set)
    vr_list.sort()

    max_vr_len = max(len(x) for x in vr_list)
    for vr in vr_list:
        vr_str = ''
        for token in vr.split('_'):
            vr_str += 'VRType::' + token + ', '
        vr_str += 'VRType::Undefined'
        vr_str = 'static const VRType {0:<{1}}[] = {{ {2} }};'.format(vr, max_vr_len, vr_str)
        print(vr_str)
    print('\n} // namespace tag_vr')

    print('')
    print('')
    print('///////////////////')
    print('// dictionary')
    print('///////////////////')

    print('struct TagDictItem')
    print('{')
    print('    TagDictItem(unsigned int a_number, const VRType* a_vr, const char* a_name, unsigned int a_vm)')
    print('        : m_number(a_number)')
    print('        , m_vm(a_vm)')
    print('        , m_vr(a_vr)')
    print('        , m_name(a_name)')
    print('    {}')
    print('')
    print('    unsigned int  m_number;')
    print('    unsigned int  m_vm = 0;')
    print('    const VRType* m_vr;')
    print('    const char*   m_name;')
    print('};')
    print('')

    print('// TODO: implement parsing of ValueMultiplicity')
    print('static const TagDictItem TagDictionary[{}] = '.format(len(tags)))
    print('{')
    for tag in tags:
        tag_str = '    {{ {0:<{1}} tag_vr::{2:<{3}} {4:<{5}} 0 }},'.format(tag.keyword+',', max_keyword_len+1, '_'.join(tag.vrs)+',', max_vr_len+1, '"'+tag.name+'",', max_name_len+3)
        print(tag_str)
    print('};')
    print('')
    print('} // namespace dict')
    print('')
    print('} // namespace dcm')
