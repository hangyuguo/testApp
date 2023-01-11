// main.js

// electron ģ�������������Ӧ�õ��������ںʹ���ԭ���������
const { app, BrowserWindow } = require('electron')
const path = require('path')

const createWindow = () => {
  // �����������
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  })

  // ���� index.html
  mainWindow.loadFile('index.html')

  // �򿪿�������
  // mainWindow.webContents.openDevTools()
}

// ��γ��򽫻��� Electron ������ʼ��
// �ʹ�����������ڵ�ʱ�����
// ���� API �� ready �¼����������ʹ�á�
app.whenReady().then(() => {
  createWindow()

  app.on('activate', () => {
    // �� macOS ϵͳ��, ���û���ѿ�����Ӧ�ô���
    // �������ͼ��ʱͨ�������´���һ���´���
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

// ���� macOS �⣬�����д��ڶ����رյ�ʱ���˳����� ���, ͨ��
// ��Ӧ�ó�������ǵĲ˵�����˵Ӧ��ʱ�̱��ּ���״̬, 
// ֱ���û�ʹ�� Cmd + Q ��ȷ�˳�
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})

// �ڵ�ǰ�ļ���������������е������̴���
// Ҳ���Բ�ֳɼ����ļ���Ȼ���� require ���롣