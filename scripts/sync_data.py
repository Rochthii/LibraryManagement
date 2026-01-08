import re
from pathlib import Path
base = Path('e:/Code/CODE/LibraryManagement')
files = base / 'files'

danhmuc_f = files / 'danhmucsach.txt'
docgia_f = files / 'docgia.txt'
muontra_f = files / 'muontra.txt'

# read danhmuc
with danhmuc_f.open('r', encoding='utf-8') as f:
    danhmuc_lines = [ln.rstrip('\n') for ln in f]

# read docgia and collect MT codes
docgia_codes = set()
with docgia_f.open('r', encoding='utf-8') as f:
    for ln in f:
        ln = ln.strip()
        if ln.startswith('MT|'):
            parts = ln.split('|')
            if len(parts) > 1 and parts[1].strip():
                docgia_codes.add(parts[1].strip())

# read muontra and classify
muontra_lines = []
unmatched = []
with muontra_f.open('r', encoding='utf-8') as f:
    for ln in f:
        s = ln.rstrip('\n')
        muontra_lines.append(s)

# build set of valid danhmuc codes
valid_codes = set()
for ln in danhmuc_lines:
    if not ln or ln.startswith('#'):
        continue
    parts = ln.split('|')
    if len(parts) >= 1:
        valid_codes.add(parts[0].strip())

# separate muontra into matched/unmatched
matched_muontra = []
for ln in muontra_lines:
    if not ln.strip():
        continue
    parts = ln.split('|')
    if len(parts) >= 2 and parts[1].strip() in valid_codes:
        matched_muontra.append(ln)
    else:
        unmatched.append(ln)

# update danhmuc statuses: change status 1->0 if code not in docgia_codes nor in matched_muontra codes
matched_mu_codes = set()
for ln in matched_muontra:
    parts = ln.split('|')
    if len(parts) >= 2:
        matched_mu_codes.add(parts[1].strip())

new_danhmuc_lines = []
changed = []
for ln in danhmuc_lines:
    if not ln:
        new_danhmuc_lines.append(ln)
        continue
    parts = ln.split('|')
    if len(parts) >= 2:
        code = parts[0].strip()
        status = parts[1].strip()
        if status == '1':
            if code not in docgia_codes and code not in matched_mu_codes:
                # change to 0
                parts[1] = '0'
                changed.append((code, '1','0'))
        new_danhmuc_lines.append('|'.join(parts))
    else:
        new_danhmuc_lines.append(ln)

# write outputs
out_danhmuc = files / 'danhmucsach_updated.txt'
with out_danhmuc.open('w', encoding='utf-8') as f:
    for ln in new_danhmuc_lines:
        f.write(ln + '\n')

out_muontra_matched = files / 'muontra_matched.txt'
with out_muontra_matched.open('w', encoding='utf-8') as f:
    for ln in matched_muontra:
        f.write(ln + '\n')

out_muontra_unmatched = files / 'muontra_unmatched.txt'
with out_muontra_unmatched.open('w', encoding='utf-8') as f:
    for ln in unmatched:
        f.write(ln + '\n')

# print a short report
print('docgia_codes:', len(docgia_codes))
print('valid_danhmuc_codes:', len(valid_codes))
print('muontra total:', len(muontra_lines))
print('muontra matched:', len(matched_muontra))
print('muontra unmatched:', len(unmatched))
print('danhmuc status changed:', len(changed))
for c,old,new in changed[:50]:
    print('CHANGED', c, old, '->', new)
print('\nWrote:')
print(out_danhmuc)
print(out_muontra_matched)
print(out_muontra_unmatched)
