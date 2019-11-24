export ACCOUNT=meosratesuzs
export DSP_ENDPOINT=https://kylin-dsp-1.liquidapps.io

cleos wallet unlock --password <./wallet_password.pwd

# Create account and import key
# curl http://faucet-kylin.blockzone.net/create/$ACCOUNT > meosratesuzs_keys.json
# export ACTIVE_PRIVATE_KEY=`cat meosratesuzs_keys.json | jq -e '.keys.active_key.private'`
# export ACTIVE_PUBLIC_KEY=`cat meosratesuzs_keys.json | jq -e '.keys.active_key.public'`
# cleos wallet import --private-key 5J7G3PoVFnQcVvtKBCa...

# # Get some tokens, stake CPU/NET, buy RAM for contract
curl http://faucet-kylin.blockzone.net/get_token/$ACCOUNT
curl http://faucet-kylin.blockzone.net/get_token/$ACCOUNT
# cleos -u $DSP_ENDPOINT system buyram $ACCOUNT $ACCOUNT "100.0000 EOS" -p $ACCOUNT@active
# cleos -u $DSP_ENDPOINT system delegatebw $ACCOUNT $ACCOUNT "20.0000 EOS" "80.0000 EOS" -p $ACCOUNT@active

export KYLIN_TEST_PUBLIC_KEY=EOS8bSKYyzWj2aS3Gz3YCm5tdaVnkaUDGvS7ERGuxeJt5jZgodVUK

export PROVIDER=heliosselene
export PACKAGE_ID=oracleservic

# select your package: 

# Stake your DAPP to the DSP that you selected the service package for:
# cleos -u $DSP_ENDPOINT push action dappservices stake "[\"$KYLIN_TEST_ACCOUNT\",\"$PROVIDER\",\"$SERVICE\",\"10.0000 DAPP\"]" -p $KYLIN_TEST_ACCOUNT@active

# # # select packages - vram and vaccounts
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"ipfsservice1\",\"package\":\"package1\"}" -p $KYLIN_TEST_ACCOUNT@active
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"package\":\"package1\"}" -p $KYLIN_TEST_ACCOUNT@active
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "{\"owner\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"package\":\"package1\"}" -p $KYLIN_TEST_ACCOUNT@active

# export SERVICE=oracleservic
# cleos -u $DSP_ENDPOINT push action dappservices selectpkg "[\"$KYLIN_TEST_ACCOUNT\",\"$PROVIDER\",\"$SERVICE\",\"$PACKAGE_ID\"]" -p $KYLIN_TEST_ACCOUNT@active

# # stake DAPP tokens
# cleos -u $EOS_ENDPOINT push action dappservices stake "{\"from\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"ipfsservice1\",\"quantity\":\"10.0000 DAPP\"}" -p $KYLIN_TEST_ACCOUNT@owner
# cleos -u $EOS_ENDPOINT push action dappservices stake "{\"from\":\"$KYLIN_TEST_ACCOUNT\",\"provider\":\"heliosselene\",\"service\":\"accountless1\",\"quantity\":\"10.0000 DAPP\"}" -p $KYLIN_TEST_ACCOUNT@owner

# # deploy contract
# cleos -u $EOS_ENDPOINT set contract $KYLIN_TEST_ACCOUNT ./contracts/eos/whitebox -p $KYLIN_TEST_ACCOUNT@owner

# # buy ram
# cleos -u $EOS_ENDPOINT system buyram $KYLIN_TEST_ACCOUNT $KYLIN_TEST_ACCOUNT "50.0000 EOS" -p $KYLIN_TEST_ACCOUNT@owner

# cleos -u $EOS_ENDPOINT set account permission $KYLIN_TEST_ACCOUNT active "{\"threshold\":1,\"keys\":[{\"weight\":1,\"key\":\"$KYLIN_TEST_PUBLIC_KEY\"}],\"accounts\":[{\"permission\":{\"actor\":\"$KYLIN_TEST_ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $KYLIN_TEST_ACCOUNT@owner

# # stake resources

# cleos -u $EOS_ENDPOINT system delegatebw $KYLIN_TEST_ACCOUNT $KYLIN_TEST_ACCOUNT "250.0 EOS" "200.0 EOS" -p $KYLIN_TEST_ACCOUNT@owner