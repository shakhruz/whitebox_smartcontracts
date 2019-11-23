export ACCOUNT=meosratesxxx
export KYLIN_TEST_PUBLIC_KEY=EOS6arWo1BxxFyoCvecQND7sWPqtLM8ErjYNwjH7yHnyTm4HL2BBJ
export DSP_ENDPOINT=https://kylin-dsp-1.liquidapps.io
export EOS_ENDPOINT=https://kylin-dsp-2.liquidapps.io

cleos wallet unlock --password <./wallet_password.pwd
# cleos wallet import --private-key 5JdBMimdQBEJ4BMzszgZKf7z...

# cleos -u $EOS_ENDPOINT set account permission $ACCOUNT active "{\"threshold\":1,\"keys\":[{\"weight\":1,\"key\":\"$KYLIN_TEST_PUBLIC_KEY\"}],\"accounts\":[{\"permission\":{\"actor\":\"$ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $ACCOUNT@active

# deploy contract
cleos -u $EOS_ENDPOINT set contract $ACCOUNT ./contracts/eos/oracleconsumer -p $ACCOUNT@active

# export CHAIN_ID=5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191
# cleos -u $DSP_ENDPOINT push action $ACCOUNT xvinit "[\"$CHAIN_ID\"]" -p $ACCOUNT

# cleos -u $DSP_ENDPOINT push action $ACCOUNT erasetoken "[\"1.0000 MEOS\"]" -p $ACCOUNT
# cleos -u $DSP_ENDPOINT push action $ACCOUNT create "{\"issuer\":\"meoswalletxx\", \"maximum_supply\":\"1000000000000 MEOS\"}" -p $ACCOUNT

# export PROVIDER=heliosselene
# export PACKAGE_ID=oracleservic

# select your package: 
# export SERVICE=oracleservic
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "[\"$ACCOUNT\",\"$PROVIDER\",\"$SERVICE\",\"$PACKAGE_ID\"]" -p $ACCOUNT@active

# Stake your DAPP to the DSP that you selected the service package for:
# cleos -u $DSP_ENDPOINT push action dappservices stake "[\"$ACCOUNT\",\"$PROVIDER\",\"$SERVICE\",\"10.0000 DAPP\"]" -p $ACCOUNT@active