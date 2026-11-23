"""Verify Speed fields and native getter loads in BOTH supplied iOS clients."""
import argparse,json,hashlib,struct
from pathlib import Path
from dump_index import parse
from verify_ios_native import MachO

def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--vng-dump',required=True);p.add_argument('--global-dump',required=True)
 p.add_argument('--vng-framework',required=True);p.add_argument('--global-framework',required=True);p.add_argument('--output',type=Path)
 a=p.parse_args();out={}
 for client,dump,binary in [('vng',a.vng_dump,a.vng_framework),('global',a.global_dump,a.global_framework)]:
  idx=parse(dump);image=MachO(binary);unit=idx[('PlayTogether.Kinematic','KinematicControllerUnit')]
  fields={}
  for name,off in [('_stableMoveSpd',0x48),('_stableAirMoveSpeed',0x7c)]:
   assert unit['fields'][name]=={'type':'float','offset':off};fields[name]=unit['fields'][name]
  assert idx[('','ActorControl')]['fields']['kunit']['offset']==0xa0
  getters={}
  for name,pattern in [('get_maxStableMoveSpeed','004840bd'),('get_maxAirMoveSpeed','007c40bd')]:
   method=next(m for m in unit['methods'] if m['name']==name);code=image.read(method['rva'],60)
   assert any(code[i:i+4]==bytes.fromhex(pattern) for i in range(0,len(code)-3,4))
   getters[name]={'rva':hex(method['rva']),'native_prefix_hex':code.hex(),'verified_load':'LDR S0, [X0, #'+('0x48' if 'Stable' in name else '0x7c')+']'}
  out[client]={'dump_sha256':hashlib.sha256(Path(dump).read_bytes()).hexdigest(),'framework_sha256':image.sha256,'fields':fields,'controller_field':{'name':'ActorControl.kunit','offset':'0xa0'},'getters':getters}
 result={'ok':True,'clients':out,'scope':'field identities and native getter loads; no in-game speed/account safety validation'}
 if a.output:a.output.write_text(json.dumps(result,indent=2)+'\n')
 print(json.dumps({'ok':True,'clients':list(out),'field_offsets':['0x48','0x7c'],'native_loads_checked':4}))
if __name__=='__main__':main()
