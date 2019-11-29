export ACCOUNT=pro1registry
export DSP_ENDPOINT=https://kylin-dsp-1.liquidapps.io

cleos wallet unlock --password <./wallet_password.pwd

# Create account and import key
# curl http://faucet-kylin.blockzone.net/create/$ACCOUNT > pro1registry_keys.json
# export ACTIVE_PRIVATE_KEY=`cat pro1registry_keys.json | jq -e '.keys.active_key.private'`
# export ACTIVE_PUBLIC_KEY=`cat pro1registry_keys.json | jq -e '.keys.active_key.public'`
# cleos wallet import --private-key $ACTIVE_PRIVATE_KEY
# cleos wallet import --private-key 5K6e1YqLdf3t76aZk7BJR56iCN7...
# curl http://faucet-kylin.blockzone.net/get_token/$ACCOUNT
# curl http://faucet-kylin.blockzone.net/get_token/$ACCOUNT
# cleos -u $DSP_ENDPOINT system buyram $ACCOUNT $ACCOUNT "100.0000 EOS" -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT system delegatebw $ACCOUNT $ACCOUNT "20.0000 EOS" "80.0000 EOS" -p $ACCOUNT@active

# export KYLIN_TEST_PUBLIC_KEY=$ACTIVE_PUBLIC_KEY
export KYLIN_TEST_PUBLIC_KEY=EOS4xAjxh1H5QVTjokWGXJZtARGUPJ95Cdr61fVCUgd3DAZtZVFX6

cleos -u $DSP_ENDPOINT set contract $ACCOUNT ./contracts/eos/registry -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT set account permission $ACCOUNT active "{\"threshold\":1,\"keys\":[{\"weight\":1,\"key\":\"$KYLIN_TEST_PUBLIC_KEY\"}],\"accounts\":[{\"permission\":{\"actor\":\"$ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"ipfsservice1\",\"package\":\"package1\"}" -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"package\":\"accountless1\"}" -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT push action dappservices stake "{\"from\":\"$ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"ipfsservice1\",\"quantity\":\"100.0000 DAPP\"}" -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT push action dappservices stake "{\"from\":\"$ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"quantity\":\"100.0000 DAPP\"}" -p $ACCOUNT@active

# export CHAIN_ID=5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191
# cleos -u $DSP_ENDPOINT push action $ACCOUNT xvinit "[\"$CHAIN_ID\"]" -p $ACCOUNT