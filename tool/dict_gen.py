import xml.etree.ElementTree as ET
import urllib2

#parse tag number given in format: '(XXXX,YYYY)'
def parse_tag_num(str):
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

def parse_vr(str):
    vr_list = ['AE', 'AS', 'AT', 'CS', 'DA', 'DS', 'DT', 'FL', 'FD', 'IS', 'LO', 'LT', 'OB', 'OD', 'OF', 'OL', 'OW', 'PN', 'SH', 'SL', 'SQ', 'SS', 'ST', 'TM', 'UC', 'UI', 'UL', 'UN', 'UR', 'US', 'UT']
    result = ''
    tokens = str.upper().split(' ')
    for token in tokens:
        if (token == 'OR'):
            continue
        if token not in vr_list:
            raise Exception('unknown VR')
        if (len(result) > 0):
            result += '|'
        result += token
    return result

def download_xml_dict(num_attempts):
    print 'Downloading dictionary...'
    for _ in range(num_attempts):
        try:
            response = urllib2.urlopen('http://dicom.nema.org/medical/dicom/current/source/docbook/part06/part06.xml')
            xml_content = response.read()
            result = ET.fromstring(xml_content)
            return result
        except Exception as e:
            print e
            continue
    print 'Failed to download dictionary'

if __name__ == '__main__':
    root = download_xml_dict(3)

    print 'Processing...'
    nsps = {'ns' : 'http://docbook.org/ns/docbook'} #namespaces
    items = root.findall('ns:chapter/ns:table/ns:tbody/ns:tr', nsps)
    dict = []
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
            tag_number = parse_tag_num(text_in_cols[0])
            tag_vr = parse_vr(text_in_cols[3])
        except Exception as e:
            print 'failed to parse tag ', text_in_cols, ':', e
            continue

        tag_desc = (tag_number, tag_vr, text_in_cols[2])
        dict.append(tag_desc)

    dict.sort(key = lambda x: x[0])

    print 'Dictionary:'

    for tag in dict:
        #tag_str = '(0x{0:04x},0x{1:04x}), {2}, {3}'.format(tag[0][0], tag[0][1], tag[1], tag[2]);
        tag_str = 'TagDef(0x{0:04x},0x{1:04x}, {2})'.format(tag[0][0], tag[0][1], tag[2]);
        print tag_str


    for tag in dict:
        tag_str = '{{ {0}, {1}, "{2}" }},'.format(tag[2], tag[1], tag[2]);
        print tag_str
