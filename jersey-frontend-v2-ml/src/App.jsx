import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom'
import { Toaster } from 'react-hot-toast'
import { AuthProvider, useAuth } from './context/AuthContext'
import Navbar   from './components/Navbar'
import Home     from './pages/Home'
import Shop     from './pages/Shop'
import Cart     from './pages/Cart'
import Admin    from './pages/Admin'
import Login    from './pages/Login'
import Register from './pages/Register'

function ProtectedRoute({ children, adminOnly = false }) {
  const { user, loading } = useAuth()
  if (loading) return null
  if (!user)   return <Navigate to="/login" replace />
  if (adminOnly && user.role !== 'admin') return <Navigate to="/" replace />
  return children
}

function AppRoutes() {
  return (
    <>
      <Navbar />
      <Routes>
        <Route path="/login"    element={<Login />} />
        <Route path="/register" element={<Register />} />
        <Route path="/" element={
          <ProtectedRoute><Home /></ProtectedRoute>
        }/>
        <Route path="/jerseys" element={
          <ProtectedRoute><Shop /></ProtectedRoute>
        }/>
        <Route path="/cart" element={
          <ProtectedRoute><Cart /></ProtectedRoute>
        }/>
        <Route path="/admin" element={
          <ProtectedRoute adminOnly><Admin /></ProtectedRoute>
        }/>
        <Route path="*" element={<Navigate to="/" replace />} />
      </Routes>
    </>
  )
}

export default function App() {
  return (
    <AuthProvider>
      <BrowserRouter>
        <AppRoutes />
        <Toaster
          position="bottom-right"
          toastOptions={{
            style: {
              background: 'var(--bg-raised)',
              color:      'var(--text-primary)',
              border:     '1px solid var(--border)',
              fontFamily: 'var(--font-body)',
              fontSize:   '0.875rem',
            },
            success: { iconTheme: { primary: '#b5ff4d', secondary: '#000' } },
          }}
        />
      </BrowserRouter>
    </AuthProvider>
  )
}
