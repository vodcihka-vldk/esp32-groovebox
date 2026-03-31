import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  base: '/esp32-groovebox/',
  plugins: [react()],
  resolve: {
    alias: {
      '@shared': '../shared',
      '@': './src'
    }
  },
  server: {
    port: 3000,
    host: true
  },
  build: {
    outDir: 'dist',
    sourcemap: true
  }
})
