#!/usr/bin/env python3
#
# Copyright (C) 2015-2021 Matthias Klumpp <mak@debian.org>
#
# SPDX-License-Identifier: LGPL-2.1+

#
# Script to download updated static data from the web and process it for AppStream to use.
#

import os
import json
import requests
import subprocess
import yaml
from datetime import date
from tempfile import TemporaryDirectory


IANA_TLD_LIST_URL = 'https://data.iana.org/TLD/tlds-alpha-by-domain.txt'
SPDX_REPO_URL = 'https://github.com/spdx/license-list-data.git'
MENU_SPEC_URL = 'https://gitlab.freedesktop.org/xdg/xdg-specs/raw/master/menu/menu-spec.xml'


def update_tld_list(url, fname):
    print('Getting TLD list from IANA...')

    data_result = list()
    r = requests.get(url)
    raw_data = str(r.content, 'utf-8')
    for line in raw_data.split('\n'):
        line = line.strip()
        if not line:
            continue
        if line.startswith('#'):
            continue

        # filter out internationalized names, we don't support them for AppStream IDs
        if line.startswith('XN--'):
            continue

        # we disallow the very long names (usually brands)
        # FIXME: Do we really want to impose this restriction just to keep the TLD
        # pool small?
        if len(line) > 4:
            continue

        data_result.append(line.lower())

    data_result.sort()
    with open(fname, 'w') as f:
        f.write('# IANA TLDs we recognize for AppStream IDs.\n')
        f.write('# Derived from the full list at {}\n'.format(url))
        f.write('# Last updated on {}\n'.format(date.today().isoformat()))

        f.write('\n'.join(data_result))
        f.write('\n')


def _read_spdx_licenses(data_dir, last_tag_ver, only_free=False):
    # load license and exception data
    licenses_json_fname = os.path.join(data_dir, 'json', 'licenses.json')
    exceptions_json_fname = os.path.join(data_dir, 'json', 'exceptions.json')
    with open(licenses_json_fname, 'r') as f:
        licenses_data = json.loads(f.read())
    with open(exceptions_json_fname, 'r') as f:
        exceptions_data = json.loads(f.read())

    # get version of the data we are currently retrieving
    license_ver_ref = licenses_data.get('licenseListVersion')
    if not license_ver_ref:
        license_ver_ref = last_tag_ver
    exceptions_ver_ref = exceptions_data.get('licenseListVersion')
    if not license_ver_ref:
        exceptions_ver_ref = last_tag_ver

    lid_list = []
    for license in licenses_data['licenses']:
        if only_free:
            if not license.get('isFsfLibre') and not license.get('isOsiApproved'):
                continue
        lid_list.append(license['licenseId'])

    eid_list = []
    for exception in exceptions_data['exceptions']:
        eid_list.append(exception['licenseExceptionId'])

    return {'licenses': lid_list,
            'exceptions': eid_list,
            'license_list_ver': license_ver_ref,
            'eceptions_list_ver': exceptions_ver_ref}


def update_spdx_id_list(git_url, licenselist_fname, licenselist_free_fname, exceptionlist_fname, with_deprecated=True):
    print('Updating list of SPDX license IDs...')
    tdir = TemporaryDirectory(prefix='spdx_master-')

    subprocess.check_call(['git',
                           'clone',
                           git_url, tdir.name])
    last_tag_ver = subprocess.check_output(['git', 'describe', '--abbrev=0',  '--tags'], cwd=tdir.name)
    last_tag_ver = str(last_tag_ver.strip(), 'utf-8')
    if last_tag_ver.startswith('v'):
        last_tag_ver = last_tag_ver[1:]

    license_data = _read_spdx_licenses(tdir.name, last_tag_ver)
    lid_list = license_data['licenses']
    eid_list = license_data['exceptions']
    license_list_ver = license_data['license_list_ver']

    lid_list.sort()
    with open(licenselist_fname, 'w') as f:
        f.write('# The list of all licenses recognized by SPDX, v{}\n'.format(license_list_ver))
        f.write('\n'.join(lid_list))
        f.write('\n')

    eid_list.sort()
    with open(exceptionlist_fname, 'w') as f:
        f.write('# The list of license exceptions recognized by SPDX, v{}\n'.format(license_data['eceptions_list_ver']))
        f.write('\n'.join(eid_list))
        f.write('\n')

    license_free_data = _read_spdx_licenses(tdir.name, last_tag_ver, only_free=True)
    with open(licenselist_free_fname, 'w') as f:
        f.write('# The list of free (OSI or FSF approved) licenses recognized by SPDX, v{}\n'.format(license_list_ver))
        f.write('\n'.join(sorted(license_free_data['licenses'])))
        f.write('\n')


def update_platforms_data():
    print('Updating platform triplet part data...')

    with open('platforms.yml', 'r') as f:
        data = yaml.safe_load(f)

    def write_platform_data(fname, values):
        with open(fname, 'w') as f:
            f.write('# This file is derived from platforms.yml - DO NOT EDIT IT MANUALLY!\n')
            f.write('\n'.join(values))
            f.write('\n')

    write_platform_data('platform_arch.txt', data['architectures'])
    write_platform_data('platform_os.txt', data['os_kernels'])
    write_platform_data('platform_env.txt', data['os_environments'])


def update_categories_list(spec_url, cat_fname):
    ''' The worst parser ever, extracting category information directoly from the spec Docbook file '''
    from enum import Enum, auto

    req = requests.get(spec_url)

    class SpecSection(Enum):
        NONE = auto()
        MAIN_CATS = auto()
        MAIN_CATS_BODY = auto()
        EXTRA_CATS = auto()
        EXTRA_CATS_BODY = auto()

    def get_entry(line):
        start = line.index('<entry>') + 7
        end = line.index('</entry>')
        return line[start:end].strip()

    main_cats = []
    extra_cats = []

    current_cat = {}
    spec_sect = SpecSection.NONE
    for line in str(req.content, 'utf-8').splitlines():
        if '<entry>Main Category</entry>' in line:
            spec_sect = SpecSection.MAIN_CATS
            continue

        if '<entry>Additional Category</entry>' in line:
            spec_sect = SpecSection.EXTRA_CATS
            continue

        if '<tbody>' in line:
            current_cat = {}
            if spec_sect == SpecSection.MAIN_CATS:
                spec_sect = SpecSection.MAIN_CATS_BODY
            else:
                spec_sect = SpecSection.EXTRA_CATS_BODY
            continue

        if spec_sect == SpecSection.MAIN_CATS_BODY:
            if '<row>' in line:
                if current_cat:
                    main_cats.append(current_cat)
                    current_cat = {}
                continue
            if '</tbody>' in line:
                if current_cat:
                    main_cats.append(current_cat)
                    current_cat = {}
                spec_sect = SpecSection.NONE
                continue

            if '<entry>' in line:
                if current_cat.get('desc'):
                    continue
                if current_cat:
                    current_cat['desc'] = get_entry(line)
                else:
                    current_cat['name'] = get_entry(line)
            continue

        if spec_sect == SpecSection.EXTRA_CATS_BODY:
            if '<row>' in line:
                if current_cat:
                    extra_cats.append(current_cat)
                    current_cat = {}
                continue
            if '</tbody>' in line:
                if current_cat:
                    main_cats.append(current_cat)
                    current_cat = {}
                spec_sect = SpecSection.NONE
                # nothing interesting follows for us after the additional categories are done
                break

            if '<entry>' in line:
                if current_cat.get('rel'):
                    continue
                if current_cat:
                    if not current_cat.get('desc'):
                        current_cat['desc'] = get_entry(line)
                    if not current_cat.get('rel'):
                        current_cat['rel'] = get_entry(line)
                else:
                    current_cat['name'] = get_entry(line)
            continue

    all_cat_names = [cat['name'] for cat in main_cats]
    all_cat_names.extend([cat['name'] for cat in extra_cats])
    all_cat_names.sort()

    with open(cat_fname, 'w') as f:
        f.write('# Freedesktop Menu Categories\n')
        f.write('# See https://specifications.freedesktop.org/menu-spec/latest/apa.html\n')
        f.write('\n'.join(all_cat_names))
        f.write('\n')


def main():
    data_dir = os.path.dirname(os.path.abspath(__file__))
    print('Data directory is: {}'.format(data_dir))
    os.chdir(data_dir)

    update_tld_list(IANA_TLD_LIST_URL, 'iana-filtered-tld-list.txt')
    update_spdx_id_list(SPDX_REPO_URL, 'spdx-license-ids.txt', 'spdx-free-license-ids.txt', 'spdx-license-exception-ids.txt')
    update_categories_list(MENU_SPEC_URL, 'xdg-category-names.txt')
    update_platforms_data()

    print('All done.')


if __name__ == '__main__':
    main()
