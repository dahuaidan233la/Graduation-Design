<template>
    <div id="bg">
        <div id="home">
            <div class="sidebar">
                <h1 class="title">实验数据管理</h1>
                <div class="addbox">
                    <input type="text" placeholder="请输入实验组名称" class="addinput" v-model="inputvalue">
                    <button class="add" @click="addMenu"> + 创建新实验</button>
                </div>
                <div class="menu">
                    <div class="item" v-for="(item, index) in menu" :key="item.id"
                        :class="{ active: activeid == item.id }" @click="changeMenu(item.id)">{{ item.name }}</div>
                </div>
            </div>
            <div class="content">
                <Connect :key="num"/>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref } from 'vue';
import { ElMessage } from 'element-plus';
import Connect from './Connect.vue';

const Sid = ref(1)
const activeid = ref(0)
const num = ref(0)
const menu = ref([{
    name: '实验组1',
    id: 0
}])
const inputvalue = ref('')

const addMenu = () => {
    if (!inputvalue.value) {
        ElMessage({
            message: '请输入实验组名称',
            type: 'warning',
        })
        return
    }
    const obj = {
        name: inputvalue.value,
        Sid: Sid.value++
    }
    menu.value.push(obj)
    console.log(menu);

}

const changeMenu = (id) => {
    activeid.value = id
    num.value += 1
}
</script>

<style scoped lang="less">
#bg {
    width: 100vw;
    height: 100vh;
    background-color: #f0f2f5;
    padding: 20px;
}

#home {
    width: 100%;
    height: 100%;
    display: flex;
}

.sidebar {
    width: 20%;
    height: 100%;
    background-color: #fff;
    border-right: 20px solid #f0f1fe;

    .title {
        text-align: center;
        padding: 20px 0;
        font-weight: 700;
        font-size: 35px;
        border-bottom: 5px solid #f0f1fe;
    }

    .addbox {
        width: 90%;
        box-sizing: border-box;
        margin: 20px auto 0;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        border-radius: 50px;

        .addinput {
            margin: 0;
            border: 0;
            font-size: 20px;
            outline: none;
            background-color: transparent;
            text-align: center;
            margin-bottom: 10px;
            background-color: #ddd;
            padding: 10px;
            border-radius: 5px;
        }

        .add {
            width: 200px;
            height: 60px;
            background-color: #fff;
            border: 1px solid #434cf3;
            border-radius: 25px;
            font-size: 20px;
            color: #434cf3;
            cursor: pointer;

            &:hover {
                background-color: #434cf3;
                color: #fff;
            }
        }
    }

    .menu {
        width: 100%;
        height: 80%;
        overflow-y: auto;
        padding: 20px;

        .item {
            margin: 10px auto;
            width: 80%;
            background-color: #fff;
            text-align: center;
            line-height: 40px;
            font-size: 20px;
            font-weight: 700;
            cursor: pointer;
            color: #666666;
            padding: 20px;
            border-radius: 15px;
            font-size: 25px;
            margin-bottom: 20px;
        }

        .active {
            background-color: #434cf3;
            color: #fff;
        }
    }
}

.content {
    flex: 1;
    background-color: #fff;
    overflow: auto;
    padding: 20px;
}
</style>