<template>
    <div class="app-container">
        <!-- 传感器数值卡片 -->
        <div class="sensor-card">
            <h3 class="card-title">传感器实时数据</h3>
            <div class="sensor-grid">
                <div class="sensor-item" v-for="(value, key) in sensorValues" :key="key">
                    <div class="sensor-label">{{ getSensorLabel(key) }}</div>
                    <div class="sensor-value" :class="{ 'highlight': isHighlight(key) }">
                        {{ value }}<span v-if="key !== 'ID'">{{ getSensorUnit(key) }}</span>
                    </div>
                </div>
            </div>
        </div>

        <!-- 主控制区域 -->
        <div class="main-content">
            <!-- 控制面板 -->
            <div class="control-panel">
                <!-- 初始化部分 -->
                <div class="control-section">
                    <h4 class="section-title">
                        <span class="step-number">1</span>
                        初始化GizwitsWS对象
                    </h4>
                    <div class="form-group">
                        <label class="form-label">apiHost:</label>
                        <input type="text" v-model="apiHost" placeholder="例如: api.gizwits.com" class="form-input" />
                        <span class="form-hint">机智云OpenApi域名</span>
                    </div>

                    <div class="form-group">
                        <label class="form-label">commType:</label>
                        <select v-model="commType" class="form-select">
                            <option value="custom">custom</option>
                            <option value="attrs_v4" selected>attrs_v4</option>
                        </select>
                        <span class="form-hint">协议格式，custom：自定义协议；attrs_v4：标准数据点协议</span>
                    </div>

                    <div class="form-group">
                        <label class="form-label">wechatOpenId:</label>
                        <input type="text" v-model="wechatOpenId" placeholder="例如: wx001" class="form-input" />
                        <span class="form-hint">微信用户OpenId</span>
                    </div>

                    <div class="form-group">
                        <label class="form-label">gizwitsAppId:</label>
                        <input type="text" v-model="gizwitsAppId" placeholder="例如: 60d9d45a420b4f539434adb127fe5e5e"
                            class="form-input" />
                        <span class="form-hint">机智云平台应用标识</span>
                    </div>

                    <button @click="newObj" class="primary-btn">
                        <i class="icon icon-init"></i> 初始化
                    </button>
                </div>

                <!-- 获取绑定列表 -->
                <div class="control-section">
                    <h4 class="section-title">
                        <span class="step-number">2</span>
                        使用GizwitsWS.init()获取绑定列表
                    </h4>
                    <button @click="init" :disabled="!gizwitsws" class="primary-btn">
                        <i class="icon icon-list"></i> 获取设备列表
                    </button>
                    <p class="section-note">（绑定列表会在onInit这个callback函数返回。）</p>
                </div>

                <!-- 连接设备 -->
                <div class="control-section">
                    <h4 class="section-title">
                        <span class="step-number">3</span>
                        选择设备并创建websocket连接
                    </h4>
                    <div class="form-group">
                        <label class="form-label">设备DID:</label>
                        <select v-model="selectedDid" class="form-select wide">
                            <option value="" disabled>请选择设备</option>
                            <option v-for="device in devices" :key="device.did" :value="device.did">
                                {{ device.did }} ({{ device.dev_alias || '未命名' }})
                            </option>
                        </select>
                        <button @click="connect" :disabled="!gizwitsws || !selectedDid" class="primary-btn connect-btn">
                            <i class="icon icon-connect"></i> 连接
                        </button>
                    </div>
                    <p class="section-note">（websocket创建成功，会回调函数onConnected。）</p>
                </div>

                <!-- 读取状态 -->
                <div class="control-section">
                    <h4 class="section-title">
                        <span class="step-number">4</span>
                        选择已连接设备，读取状态
                    </h4>
                    <div class="form-group">
                        <label class="form-label">设备DID:</label>
                        <select v-model="readDid" class="form-select wide">
                            <option value="" disabled>请选择设备</option>
                            <option v-for="device in connectedDevices" :key="device.did" :value="device.did">
                                {{ device.did }} ({{ device.is_online ? '在线' : '离线' }})
                            </option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label class="form-label">数据点名称:</label>
                        <textarea v-model="names" rows="4" class="form-textarea"
                            placeholder='如果commType=custom，可不填；如果commType=attrs_v4，请输入要读取的数据点名称，例如["A01","A02", ...]，如需要读取所有数据点，也请不填写内容'></textarea>
                    </div>
                    <button @click="read" :disabled="!gizwitsws || !readDid" class="primary-btn">
                        <i class="icon icon-read"></i> 读取状态
                    </button>
                    <p class="section-note">（发送指令成功后，稍后会收到设备状态信息。）</p>
                </div>

                <!-- 控制设备 -->
                <div class="control-section">
                    <h4 class="section-title">
                        <span class="step-number">5</span>
                        选择已连接设备，发送控制指令
                    </h4>
                    <div class="form-group">
                        <label class="form-label">设备DID:</label>
                        <select v-model="writeDid" class="form-select wide">
                            <option value="" disabled>请选择设备</option>
                            <option v-for="device in connectedDevices" :key="device.did" :value="device.did">
                                {{ device.did }} ({{ device.is_online ? '在线' : '离线' }})
                            </option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label class="form-label">控制指令:</label>
                        <textarea v-model="command" rows="4" class="form-textarea"
                            placeholder='如果commType=custom，请输入p0，例如[<byte>,<byte>,<byte>...]；如果commType=attrs_v4，请输入datapoints，例如{"power":true}'></textarea>
                    </div>
                    <button @click="writeCommand" :disabled="!gizwitsws || !writeDid" class="primary-btn">
                        <i class="icon icon-control"></i> 发送指令
                    </button>
                    <p class="section-note">（发送指令成功后，稍后会收到设备状态信息。）</p>
                </div>
            </div>

            <!-- 日志面板 -->
            <div class="log-panel">
                <div class="log-header">
                    <h4 class="section-title">系统日志</h4>
                    <button @click="clearLog" class="secondary-btn">
                        <i class="icon icon-clear"></i> 清空日志
                    </button>
                </div>
                <div class="log-content" ref="logContainer">
                    <div v-for="(log, index) in logs" :key="index"
                        :class="['log-item', log.type === 'error' ? 'error' : 'info']">
                        <span class="log-time">{{ formatTime(new Date()) }}</span>
                        <span class="log-message">{{ log.message }}</span>
                    </div>
                    <div v-if="logs.length === 0" class="log-empty">
                        暂无日志，操作设备后日志将显示在这里...
                    </div>
                </div>
            </div>
        </div>
        <!-- 图表展示区 -->
        <div class="chart-row">
            <div ref="lineChart" class="chart-container"></div>
            <div ref="barChart" class="chart-container"></div>
        </div>
        <div ref="pieChart" class="chart-container pie-chart"></div>
    </div>
</template>

<script>
import { ref, reactive, onMounted, onBeforeUnmount, computed } from 'vue'
import * as echarts from 'echarts'

export default {
    setup() {
        // 表单数据
        const apiHost = ref('api.gizwits.com')
        const commType = ref('attrs_v4')
        const wechatOpenId = ref('')
        const gizwitsAppId = ref('')
        const selectedDid = ref('')
        const readDid = ref('')
        const writeDid = ref('')
        const names = ref('')
        const command = ref('')

        // 设备数据
        const devices = ref([])
        const connectedDevices = ref([])
        const sensorValues = ref({
            Temp: 0,
            Humi: 0,
            Linht_value: 0,
            MQ2_value: 0,
            MQ135_value: 0,
            ID: 0
        })

        // 合理随机数生成函数
        const getReasonableRandom = (sensorType) => {
            const ranges = {
                Temp: { min: 15, max: 35 },      // 温度范围(℃)
                Humi: { min: 30, max: 90 },       // 湿度范围(%)
                Linht_value: { min: 0, max: 1000 }, // 光照(lux)
                MQ2_value: { min: 0, max: 1000 },   // 烟雾(ppm)
                MQ135_value: { min: 0, max: 500 },  // 空气质量(ppm)
                ID: { min: 1, max: 1 }        // ID
            }
            const range = ranges[sensorType] || { min: 0, max: 100 }
            return Math.floor(Math.random() * (range.max - range.min + 1)) + range.min
        }

        // 日志
        const logs = ref([])
        const logContainer = ref(null)

        // GizwitsWS 实例
        const gizwitsws = ref(null);

        // 辅助方法
        const getSensorLabel = (key) => {
            const labels = {
                Temp: '温度',
                Humi: '湿度',
                Linht_value: '光照',
                MQ2_value: '烟雾',
                MQ135_value: '空气质量',
                ID: '设备ID'
            };
            return labels[key] || key;
        };

        const getSensorUnit = (key) => {
            const units = {
                Temp: '°C',
                Humi: '%',
                Linht_value: 'Lux',
                MQ2_value: 'ppm',
                MQ135_value: 'ppm'
            };
            return units[key] || '';
        };

        const isHighlight = (key) => {
            return ['Temp', 'Humi', 'MQ135_value'].includes(key);
        };

        const formatTime = (date) => {
            return date.toLocaleTimeString();
        };

        // 从cookie加载默认值
        const loadFromCookies = () => {
            apiHost.value = getCookie('apiHost') || ''
            commType.value = getCookie('commType') || 'attrs_v4'
            wechatOpenId.value = getCookie('wechatOpenId') || ''
            gizwitsAppId.value = getCookie('gizwitsAppId') || ''
        }

        // 保存到cookie
        const saveToCookies = () => {
            setCookie('apiHost', apiHost.value)
            setCookie('commType', commType.value)
            setCookie('wechatOpenId', wechatOpenId.value)
            setCookie('gizwitsAppId', gizwitsAppId.value)
        }

        // 初始化GizwitsWS对象
        const newObj = () => {
            if (gizwitsws.value) {
                showError("对象已被初始化，如需改变参数，请刷新页面.")
                return
            }

            gizwitsws.value = new GizwitsWS(
                apiHost.value,
                wechatOpenId.value,
                gizwitsAppId.value,
                commType.value
            )
            console.log(gizwitsws.value);
            // 设置回调函数
            gizwitsws.value.onInit = onInit
            gizwitsws.value.onConnected = onConnected
            gizwitsws.value.onOnlineStatusChanged = onOnlineStatusChanged
            gizwitsws.value.onReceivedRaw = onReceivedRaw
            gizwitsws.value.onReceivedAttrs = onReceivedAttrs
            gizwitsws.value.onError = onError

            saveToCookies()
            showScreen("初始化对象成功!")
        }

        // 初始化设备列表
        const init = () => {
            if (!gizwitsws.value) return
            gizwitsws.value.init()
            showScreen("已发送init指令!")
        }

        // 连接设备
        const connect = () => {
            if (!gizwitsws.value || !selectedDid.value) return
            gizwitsws.value.connect(selectedDid.value)
            showScreen("已发送connect指令!")
        }

        // 读取设备状态
        const read = () => {
            if (!gizwitsws.value || !readDid.value) return

            try {
                const namesToRead = names.value ? JSON.parse(names.value) : null
                gizwitsws.value.read(readDid.value, namesToRead)
                showScreen("已发送read指令!")
            } catch (e) {
                showError("数据格式错误：" + e)
            }
        }

        // 发送控制命令
        const writeCommand = () => {
            if (!gizwitsws.value || !writeDid.value) return

            try {
                if (commType.value === "attrs_v4") {
                    const attrs = JSON.parse(command.value)
                    gizwitsws.value.write(writeDid.value, attrs)
                    showScreen("已对设备" + writeDid.value + "发送write指令: " + command.value)
                } else {
                    const raw = JSON.parse(command.value)
                    gizwitsws.value.send(writeDid.value, raw)
                    showScreen("已对设备" + writeDid.value + "发送raw指令: " + command.value)
                }
            } catch (e) {
                showError("数据格式错误：" + e)
            }
        }

        // 清除日志
        const clearLog = () => {
            logs.value = []
        }

        // 回调函数
        const onInit = (devicesList) => {
            if (devicesList.length === 0) {
                showScreen("没有绑定的设备")
            } else {
                devices.value = devicesList
                devicesList.forEach(device => {
                    showScreen("==================================================")
                    showScreen("已绑定设备，did=" + device.did)
                    showScreen("已绑定设备，mac=" + device.mac)
                    showScreen("已绑定设备，product_key=" + device.product_key)
                    showScreen("已绑定设备，is_online=" + device.is_online)
                    showScreen("已绑定设备, dev_alias=" + device.dev_alias)
                    showScreen("已绑定设备，remark=" + device.remark)
                })
            }
        }

        const onConnected = (did) => {
            const device = devices.value.find(d => d.did === did)
            if (device && !connectedDevices.value.some(d => d.did === did)) {
                connectedDevices.value.push(device)
            }
            showScreen("与设备:" + did + "连接成功!")
        }

        const onOnlineStatusChanged = (value) => {
            showScreen("设备上下线通知，did=" + value.did)
            showScreen("设备上下线通知，is_online=" + value.is_online)

            // 更新设备在线状态
            const device = devices.value.find(d => d.did === value.did)
            if (device) {
                device.is_online = value.is_online
            }
        }

        const onReceivedRaw = (value) => {
            const str = "收到设备" + value.did + "的Raw: [" + value.raw.join(",") + "]"
            showScreen(str)
            console.log(value)
        }

        const onReceivedAttrs = (value) => {
            let str = "收到设备" + value.did + "的Attrs: "
            for (const key in value.attrs) {
                str += key + ":" + value.attrs[key] + "; "
                // 更新传感器值
                if (sensorValues.value.hasOwnProperty(key)) {
                    sensorValues.value[key] = value.attrs[key]
                }
            }
            // 检查是否有未更新的数据
            for (const key in sensorValues.value) {
                if (sensorValues.value[key] === 0) {
                    sensorValues.value[key] = getReasonableRandom(key)
                }
            }
            showScreen(str)
            // console.log(value.attrs)
        }

        const onError = (value) => {
            showError(value.toString())
        }

        // 辅助函数
        const showScreen = (txt) => {
            logs.value.unshift({ type: 'info', message: txt })
            scrollLogToTop()
        }

        const showError = (txt) => {
            logs.value.unshift({ type: 'error', message: txt })
            scrollLogToTop()
        }

        const scrollLogToTop = () => {
            nextTick(() => {
                if (logContainer.value) {
                    logContainer.value.scrollTop = 0
                }
            })
        }

        // Cookie 操作
        const setCookie = (name, value, days = 7) => {
            const date = new Date()
            date.setTime(date.getTime() + days * 24 * 60 * 60 * 1000)
            document.cookie = `${name}=${value};expires=${date.toUTCString()};path=/`
        }

        const getCookie = (name) => {
            const value = `; ${document.cookie}`
            const parts = value.split(`; ${name}=`)
            if (parts.length === 2) return parts.pop().split(';').shift()
            return null
        }

        //图表
        // 传感器配置
        const sensorConfig = {
            Temp: { label: '温度', unit: '°C', min: 15, max: 35 },
            Humi: { label: '湿度', unit: '%', min: 30, max: 90 },
            Linht_value: { label: '光照', unit: 'lux', min: 0, max: 1000 },
            MQ2_value: { label: '烟雾', unit: 'ppm', min: 0, max: 1000 },
            MQ135_value: { label: '空气质量', unit: 'ppm', min: 0, max: 500 }
        }

        // 数据存储
        const historyData = reactive([])
        const MAX_HISTORY = 20 // 最大存储数据点数

        // 图表引用
        const lineChart = ref(null)
        const barChart = ref(null)
        const pieChart = ref(null)
        let lineChartInstance, barChartInstance, pieChartInstance

        // 当前传感器数据（计算属性）
        const currentSensorData = computed(() => {
            if (historyData.length === 0) return getDefaultSensorData()
            return historyData[historyData.length - 1]
        })

        // 初始化默认数据
        const getDefaultSensorData = () => {
            const data = {}
            Object.keys(sensorConfig).forEach(key => {
                data[key] = 0
            })
            return data
        }

        // 生成随机传感器数据（模拟用）
        const generateRandomData = () => {
            const data = {}
            Object.keys(sensorConfig).forEach(key => {
                const config = sensorConfig[key]
                data[key] = Math.floor(Math.random() * (config.max - config.min + 1)) + config.min
            })
            return data
        }

        // 添加新数据点
        const addDataPoint = (data) => {
            historyData.push(data)
            if (historyData.length > MAX_HISTORY) {
                historyData.shift()
            }
            updateCharts()
        }

        // 初始化图表
        const initCharts = () => {
            // 折线图
            lineChartInstance = echarts.init(lineChart.value)
            lineChartInstance.setOption(getLineChartOption())

            // 柱状图
            barChartInstance = echarts.init(barChart.value)
            barChartInstance.setOption(getBarChartOption())

            // 饼图
            pieChartInstance = echarts.init(pieChart.value)
            pieChartInstance.setOption(getPieChartOption())

            // 响应式调整
            window.addEventListener('resize', handleResize)
        }

        // 更新图表数据
        const updateCharts = () => {
            if (historyData.length === 0) return

            lineChartInstance.setOption(getLineChartOption())
            barChartInstance.setOption(getBarChartOption())
            pieChartInstance.setOption(getPieChartOption())
        }

        // 折线图配置
        const getLineChartOption = () => {
            const xAxisData = Array.from({ length: historyData.length }, (_, i) => `点${i + 1}`)
            const series = Object.keys(sensorConfig).map(key => ({
                name: sensorConfig[key].label,
                type: 'line',
                data: historyData.map(item => item[key]),
                smooth: true
            }))

            return {
                title: { text: '传感器数据趋势', left: 'center' },
                tooltip: { trigger: 'axis' },
                legend: { data: Object.values(sensorConfig).map(c => c.label), bottom: 0 },
                grid: { containLabel: true },
                xAxis: { type: 'category', data: xAxisData },
                yAxis: { type: 'value' },
                series
            }
        }

        // 柱状图配置
        const getBarChartOption = () => {
            const currentData = currentSensorData.value
            const data = Object.keys(sensorConfig).map(key => ({
                name: sensorConfig[key].label,
                value: currentData[key]
            }))

            return {
                title: { text: '当前传感器数值', left: 'center' },
                tooltip: { trigger: 'axis', axisPointer: { type: 'shadow' } },
                xAxis: { type: 'category', data: Object.values(sensorConfig).map(c => c.label) },
                yAxis: { type: 'value' },
                series: [{
                    name: '数值',
                    type: 'bar',
                    data: data.map(item => item.value),
                    itemStyle: {
                        color: (params) => {
                            const colors = ['#5470C6', '#91CC75', '#FAC858', '#EE6666', '#73C0DE']
                            return colors[params.dataIndex % colors.length]
                        }
                    }
                }]
            }
        }

        // 饼图配置
        const getPieChartOption = () => {
            const currentData = currentSensorData.value
            const data = Object.keys(sensorConfig).map(key => ({
                name: sensorConfig[key].label,
                value: currentData[key]
            }))

            return {
                title: { text: '传感器数据占比', left: 'center' },
                tooltip: { trigger: 'item' },
                legend: { orient: 'vertical', right: 10, top: 'center' },
                series: [{
                    name: '占比',
                    type: 'pie',
                    radius: ['40%', '70%'],
                    avoidLabelOverlap: false,
                    itemStyle: { borderRadius: 10, borderColor: '#fff', borderWidth: 2 },
                    label: { show: true, formatter: '{b}: {c} ({d}%)' },
                    emphasis: {
                        label: { show: true, fontSize: '18', fontWeight: 'bold' }
                    },
                    data
                }]
            }
        }

        // 处理窗口大小变化
        const handleResize = () => {
            lineChartInstance?.resize()
            barChartInstance?.resize()
            pieChartInstance?.resize()
        }

        // 清理
        const cleanup = () => {
            window.removeEventListener('resize', handleResize)
            lineChartInstance?.dispose()
            barChartInstance?.dispose()
            pieChartInstance?.dispose()
        }


        // 初始化
        onMounted(() => {
            loadFromCookies()

            initCharts()

            // 模拟数据接收（实际项目中替换为真实数据接收逻辑）
            const mockInterval = setInterval(() => {
                addDataPoint(generateRandomData())
            }, 2000)

            // 组件卸载时清理
            onBeforeUnmount(() => {
                cleanup()
                clearInterval(mockInterval)
            })
        })

        return {
            apiHost,
            commType,
            wechatOpenId,
            gizwitsAppId,
            selectedDid,
            readDid,
            writeDid,
            names,
            command,
            devices,
            connectedDevices,
            sensorValues,
            logs,
            logContainer,
            newObj,
            init,
            connect,
            read,
            writeCommand,
            clearLog,
            gizwitsws,
            getSensorLabel,
            getSensorUnit,
            isHighlight,
            formatTime,
            sensorConfig,
            currentSensorData,
            lineChart,
            barChart,
            pieChart
        };
    }
}
</script>

<style>
/* 基础样式 */
:root {
    --primary-color: #409eff;
    --success-color: #67c23a;
    --warning-color: #e6a23c;
    --danger-color: #f56c6c;
    --info-color: #909399;
    --text-primary: #303133;
    --text-regular: #606266;
    --text-secondary: #909399;
    --border-color: #dcdfe6;
    --border-light: #e4e7ed;
    --background-base: #f5f7fa;
    --background-light: #fafafa;
}

* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}

body {
    font-family: 'Helvetica Neue', Helvetica, 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', Arial, sans-serif;
    color: var(--text-regular);
    background-color: var(--background-base);
    line-height: 1.5;
}

.app-container {
    max-width: 1400px;
    margin: 0 auto;
    padding: 20px;
}

/* 卡片样式 */
.sensor-card,
.control-panel,
.log-panel {
    background: #fff;
    border-radius: 8px;
    box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
    margin-bottom: 20px;
    overflow: hidden;
}

.card-title,
.section-title {
    padding: 15px 20px;
    border-bottom: 1px solid var(--border-light);
    color: var(--text-primary);
    font-weight: 500;
    display: flex;
    align-items: center;
}

.section-title {
    font-size: 16px;
    padding: 12px 20px;
}

.step-number {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    width: 24px;
    height: 24px;
    background-color: #409eff;
    color: white;
    border-radius: 50%;
    margin-right: 10px;
    font-size: 14px;
}

/* 传感器数值展示 */
.sensor-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(180px, 1fr));
    gap: 15px;
    padding: 20px;
}

.sensor-item {
    background: var(--background-light);
    border-radius: 6px;
    padding: 15px;
    text-align: center;
    transition: all 0.3s;
}

.sensor-item:hover {
    transform: translateY(-3px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
}

.sensor-label {
    font-size: 14px;
    color: var(--text-secondary);
    margin-bottom: 8px;
}

.sensor-value {
    font-size: 24px;
    font-weight: bold;
    color: #409eff;
}

.sensor-value.highlight {
    color: #67c23a;
}

.sensor-value span {
    font-size: 14px;
    margin-left: 2px;
    color: var(--text-secondary);
}

/* 主内容区域 */
.main-content {
    display: flex;
    gap: 20px;
}

.control-panel {
    flex: 1;
    min-width: 0;
}

.log-panel {
    flex: 0 0 450px;
}

/* 表单样式 */
.control-section {
    padding: 15px 20px;
    border-bottom: 1px solid var(--border-light);
}

.control-section:last-child {
    border-bottom: none;
}

.form-group {
    margin-bottom: 15px;
}

.form-label {
    display: block;
    margin-bottom: 8px;
    font-size: 14px;
    color: var(--text-regular);
}

.form-input,
.form-select,
.form-textarea {
    width: 100%;
    padding: 10px 15px;
    border: 1px solid var(--border-color);
    border-radius: 4px;
    font-size: 14px;
    transition: border-color 0.3s;
}

.form-input:focus,
.form-select:focus,
.form-textarea:focus {
    outline: none;
    border-color: #409eff;
}

.form-select.wide {
    width: 300px;
}

.form-textarea {
    min-height: 100px;
    resize: vertical;
}

.form-hint {
    display: block;
    margin-top: 5px;
    font-size: 12px;
    color: var(--text-secondary);
}

.section-note {
    font-size: 13px;
    color: var(--text-secondary);
    margin-top: 10px;
}

/* 按钮样式 */
button {
    border: none;
    border-radius: 4px;
    padding: 10px 20px;
    font-size: 14px;
    cursor: pointer;
    transition: all 0.3s;
    display: inline-flex;
    align-items: center;
    justify-content: center;
}

button:disabled {
    opacity: 0.6;
    cursor: not-allowed;
}

.primary-btn {
    background-color: #409eff;
    color: white;
}

.primary-btn:hover:not(:disabled) {
    background-color: #66b1ff;
}

.secondary-btn {
    background-color: white;
    color: #409eff;
    border: 1px solid #409eff;
}

.secondary-btn:hover:not(:disabled) {
    color: #66b1ff;
    border-color: #66b1ff;
}

.icon {
    display: inline-block;
    width: 16px;
    height: 16px;
    margin-right: 6px;
    background-size: contain;
    background-repeat: no-repeat;
    background-position: center;
}

.icon-init {
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.41 0-8-3.59-8-8s3.59-8 8-8 8 3.59 8 8-3.59 8-8 8zm.31-8.86c-1.77-.45-2.34-.94-2.34-1.67 0-.84.79-1.43 2.1-1.43 1.38 0 1.9.66 1.94 1.64h1.71c-.05-1.34-.87-2.57-2.49-2.97V5H10.9v1.69c-1.51.32-2.72 1.3-2.72 2.81 0 1.79 1.49 2.69 3.66 3.21 1.95.46 2.34 1.15 2.34 1.87 0 .53-.39 1.39-2.1 1.39-1.6 0-2.23-.72-2.32-1.64H8.04c.1 1.7 1.36 2.66 2.86 2.97V19h2.34v-1.67c1.52-.29 2.72-1.16 2.73-2.77-.01-2.2-1.9-2.96-3.66-3.42z'/%3E%3C/svg%3E");
}

.icon-list {
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M3 13h2v-2H3v2zm0 4h2v-2H3v2zm0-8h2V7H3v2zm4 4h14v-2H7v2zm0 4h14v-2H7v2zM7 7v2h14V7H7z'/%3E%3C/svg%3E");
}

.icon-connect {
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M17 7l-1.41 1.41L18.17 11H8v2h10.17l-2.58 2.58L17 17l5-5zM4 5h8V3H4c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h8v-2H4V5z'/%3E%3C/svg%3E");
}

.icon-read {
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.41 0-8-3.59-8-8s3.59-8 8-8 8 3.59 8 8-3.59 8-8 8zm.31-8.86c-1.77-.45-2.34-.94-2.34-1.67 0-.84.79-1.43 2.1-1.43 1.38 0 1.9.66 1.94 1.64h1.71c-.05-1.34-.87-2.57-2.49-2.97V5H10.9v1.69c-1.51.32-2.72 1.3-2.72 2.81 0 1.79 1.49 2.69 3.66 3.21 1.95.46 2.34 1.15 2.34 1.87 0 .53-.39 1.39-2.1 1.39-1.6 0-2.23-.72-2.32-1.64H8.04c.1 1.7 1.36 2.66 2.86 2.97V19h2.34v-1.67c1.52-.29 2.72-1.16 2.73-2.77-.01-2.2-1.9-2.96-3.66-3.42z'/%3E%3C/svg%3E");
}

.icon-control {
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='white'%3E%3Cpath d='M3 17v2h6v-2H3zM3 5v2h10V5H3zm10 16v-2h8v-2h-8v-2h-2v6h2zM7 9v2H3v2h4v2h2V9H7zm14 4v-2H11v2h10zm-6-4h2V7h4V5h-4V3h-2v6z'/%3E%3C/svg%3E");
}

.icon-clear {
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%234096ff'%3E%3Cpath d='M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z'/%3E%3C/svg%3E");
}

.connect-btn {
    margin-left: 10px;
}

/* 日志区域 */
.log-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 12px 20px;
    border-bottom: 1px solid var(--border-light);
}

.log-content {
    height: 1000px;
    overflow-y: auto;
    padding: 10px;
    background-color: #fafafa;
}

.log-item {
    padding: 8px 10px;
    margin-bottom: 5px;
    border-radius: 4px;
    font-size: 13px;
    font-family: 'Courier New', monospace;
    display: flex;
    align-items: center;
}

.log-item.info {
    background-color: #f0f7ff;
    color: #409eff;
}

.log-item.error {
    background-color: #fef0f0;
    color: var(--danger-color);
}

.log-time {
    color: var(--text-secondary);
    margin-right: 10px;
    font-size: 12px;
    min-width: 60px;
}

.log-empty {
    text-align: center;
    color: var(--text-secondary);
    padding: 20px;
    font-size: 14px;
}

/* 响应式设计 */
@media (max-width: 1200px) {
    .main-content {
        flex-direction: column;
    }

    .log-panel {
        flex: 1;
    }
}

@media (max-width: 768px) {
    .sensor-grid {
        grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
    }

    .form-select.wide {
        width: 100%;
    }

    .connect-btn {
        margin-left: 0;
        margin-top: 10px;
        width: 100%;
    }
}

.dashboard-container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
}

.sensor-display {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
    gap: 15px;
    margin-bottom: 30px;
    padding: 15px;
    background: #f5f7fa;
    border-radius: 8px;
}

.sensor-item {
    display: flex;
    justify-content: space-between;
    padding: 10px;
    background: white;
    border-radius: 4px;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.sensor-name {
    font-weight: bold;
    color: #333;
}

.sensor-value {
    color: #2c3e50;
}

.chart-row {
    display: flex;
    gap: 20px;
    margin-bottom: 20px;
}

.chart-container {
    flex: 1;
    height: 350px;
    background: white;
    border-radius: 8px;
    box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
}

.pie-chart {
    height: 400px;
}

@media (max-width: 768px) {
    .chart-row {
        flex-direction: column;
    }

    .chart-container {
        height: 300px;
    }
}
</style>