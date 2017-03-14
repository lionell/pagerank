#!/usr/bin/env python3

import argparse
import logging
import random

parser = argparse.ArgumentParser()
parser.add_argument('name', help='Name of generated dataset', type=str)
parser.add_argument('--page_cnt', help='Number of pages in graph', type=int)
parser.add_argument('--max_in_link_cnt',
        help='Upper bound of in-links per page', type=int, default=1000)
parser.add_argument('--output',
        help='Path to store generated dataset', type=str, default='')
parser.add_argument('--chunk_size', help='Maximum of links per chunk',
        type=int, default=10000)
parser.add_argument('--log', help='Logging level', type=str, default='INFO',
        choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'])
args = parser.parse_args()

log_level = getattr(logging, args.log.upper(), None)
logging.basicConfig(level=log_level)

if args.max_in_link_cnt > args.page_cnt - 1:
    args.max_in_link_cnt = args.page_cnt - 1
    logging.warning('Using {} as max_in_link_cnt.'.format(args.max_in_link_cnt))

path = args.output + args.name

out_link_cnts = [0] * args.page_cnt
for i in range(args.page_cnt):
    link_cnt = random.randint(0, args.max_in_link_cnt)
    links = random.sample(range(args.page_cnt), link_cnt)

    # Update out_link_cnts with current links
    for page in links:
        out_link_cnts[page] += 1

    # Open appropriate chunk file
    if i % args.chunk_size == 0:
        chunk = i // args.chunk_size
        f = open(path + '_' + str(chunk) + '.chnk', 'w')
        logging.info('Generating {}-th chunk.'.format(chunk))

    f.write(str(i) + ' ' + str(link_cnt) + ' ')
    f.write(' '.join(map(str, links)) + '\n')

    # Don't forget to close chunk file
    if (i + 1) % args.chunk_size == 0:
        f.close()

if not f.closed:
    f.close()

with open(path + '.meta', 'w') as f:
    logging.info('Writing metadata to \'{}\'.'.format(path + '.meta'))
    f.write(str(len(out_link_cnts)) + '\n')
    f.write(str(args.chunk_size) + '\n')
    f.write(' '.join(map(str, out_link_cnts)))

logging.info('Generation finished.')
